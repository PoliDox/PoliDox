#include "Server.h"
#include "ServerController.h"



static QString getIdentifier(QWebSocket *peer) {
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}


Server::Server(quint16 port, QObject *parent) : QObject(parent) {
    thread()->setObjectName("Main Thread");

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Polidox Server"),
                                              QWebSocketServer::NonSecureMode, this);

    this->dbOperations = new DatabaseManager();

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "PoliDox server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
    } else {
        qDebug() << m_pWebSocketServer->errorString();
    }
}


Account* Server::getAccount(QWebSocket *socketOfAccont){
    return this->socket2account[socketOfAccont];
}


Server::~Server() {
    m_pWebSocketServer->close();

    //TODO: vedere bene cosa altro distruggere?? ad esempio
    //      tutti gli altri elementi dinamici nella
    //      mappa socket2account ???
}


//TODO: - codice copiato da ClientController::onTextMessageReceived, RIFATTORIZZARE!!!
//      - nella creazione dell'account, inserire anche l'immagine
//        quindi checkPassword deve ritornare anche l'immagine oltre che il siteId
//      - inserire anche l'immagine nel valore di ritorno di checkPassword
//- To handle requests by not logged account.
//- The only valid actions can be "registerUser","loginReq"
//- Note that "genericRequestString" is a String type but interally
//  it's formatted in JSON
void Server::handleNotLoggedRequests(const QString& genericRequestString){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());
    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;

    requestDocJSON = QJsonDocument::fromJson(genericRequestString.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
    requestObjJSON = requestDocJSON.object();

    // No switch case for strings in C++ :((
    QString header = requestObjJSON["action"].toString();
    if (header == "loginReq") {
        QString name = requestObjJSON["name"].toString();
        QString password = requestObjJSON["password"].toString();

        bool loginSuccess = false;
        Account *loggedAccount = nullptr;
        QList<QString> nameDocuments;

        //check if account is already logged. If true, login is rejected
        bool alreadyLogged = false;
        for(Account* account : this->socket2account.values())
            if(account != nullptr && account->getName() == name)
                alreadyLogged = true;

        int result = -1;
        if(!alreadyLogged)
            result = this->dbOperations->checkPassword(name, password);
        qDebug() << "Authentication result: " << result;

        if(alreadyLogged == false && result >= 0){
            loginSuccess = true;
            //in case of success, result will contain siteId and [image(TODO!!)]
            loggedAccount = new Account(result, name, ""); //TODO : inserire anche l'immagine
            this->socket2account[signalSender] = loggedAccount;

            nameDocuments = this->dbOperations->getAllDocuments(loggedAccount->getSiteId());

            disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleNotLoggedRequests);
            connect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);
        }

        QByteArray sendMsgToClient = ServerMessageFactory::createLoginReply(loginSuccess, loggedAccount, nameDocuments);
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else if (header == "registerUser"){
        QString name = requestObjJSON["name"].toString();
        QString password = requestObjJSON["password"].toString();

        QByteArray image ;// = requestObjJSON["image"].toString();  //TODO: da sistemare, come convertire l'immagine ???
                                                                    //      vedere anche Account::toJSON()

        bool registrationSuccess = false;
        int siteId = this->dbOperations->registerUser(name, password, image);
        if(siteId >= 0)
            registrationSuccess = true;

        QByteArray sendMsgToClient = ServerMessageFactory::createRegistrationUserReply(registrationSuccess, siteId);
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }

}


ServerController* Server::initializeServerController(QString& nameDocument, QList<Char>& orderedInserts){
    ServerController *fileServContr = new ServerController(nameDocument, this);
    fileServContr->createCrdt(orderedInserts);
    return fileServContr;
}


//- To handle requests by logged account
//- The only valid actions can be "openFileReq","createFileReq"
void Server::handleLoggedRequests(const QString& genericRequestString){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());

    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;

    requestDocJSON = QJsonDocument::fromJson(genericRequestString.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
    requestObjJSON = requestDocJSON.object();

    // No switch case for strings in C++ :((
    ServerController *fileServContr = nullptr;
    QString nameDocument = requestObjJSON["nameDocument"].toString();
    QString header = requestObjJSON["action"].toString();
    if (header == "openFileReq") {
        if( !(this->file2serverController.contains(nameDocument)) ){
            QList<Char> orderedInserts = this->dbOperations->getAllInserts(nameDocument);
            fileServContr = this->initializeServerController(nameDocument, orderedInserts);

            this->file2serverController[nameDocument] = fileServContr;
        } else {
            fileServContr = this->file2serverController[nameDocument];
        }

        //TODO: gestire la open file reply nel caso in cui la richiesta
        //      non vada a buon fine

        fileServContr->addClient(signalSender);
        disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);
        disconnect(signalSender, &QWebSocket::disconnected, this, &Server::disconnectAccount);
    }
    else if (header == "createFileReq"){
        //create and open the file
        //when a user creates a file he has, by default, the permission to open it
        QString nameAccount = this->socket2account[signalSender]->getName();
        QString uriOfDocument = this->generateUri(nameAccount, nameDocument);
        bool result = this->dbOperations->insertNewDocument(nameDocument, uriOfDocument);

        bool result2 = false;
        if(result){
            Account* loggedAccount = this->socket2account[signalSender];
            result2 = this->dbOperations->insertNewPermission(nameDocument, loggedAccount->getSiteId());
        }

        if(result2){
            QList<Char> emptyList;
            fileServContr = this->initializeServerController(nameDocument, emptyList);
            this->file2serverController[nameDocument] = fileServContr;

            fileServContr->addClient(signalSender);
            disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);
        } else {
            //TODO: gestire la reply in caso di fallimento
        }

    }
    else if(header == "permissionReq"){
        QString uri = requestObjJSON["uri"].toString();
        int siteId = requestObjJSON["siteId"].toInt();      // siteId of account who is asking
                                                            // for permission
        bool response = true;
        try {
            QString nameDocument = this->dbOperations->getDocument(uri);
        } catch (std::exception e) {
            //does not exists a document with that uri   //TODO: da testare
            response = false;
            nameDocument = "";
        }

        if(response)
            this->dbOperations->insertNewPermission(nameDocument, siteId);

        QByteArray sendMsgToClient = ServerMessageFactory::createPermissionReply(response, nameDocument);
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }
}


int Server::getDigits(std::string s) {
    long long int digits = 0;
    int runningLength = 0;
    int prevLength = 0;
    for ( auto it = s.rbegin(); it != s.rend(); ++it ) {
        runningLength += prevLength;
        //if digits is a 7 digit number then appending a 3 digit number would overflow an int
        digits += (long long int)*it * pow(10, runningLength);
        //we have to work out the length of the current digit
        //so we know how much we need to shift by next time
        int dLength = 0;
        for ( int d = *it; d > 0; dLength++, d /= 10 );
        prevLength = dLength;
        if ( digits >= 100000000 ) break;
    }
    return digits % 100000000;
}


//TODO
QString Server::generateUri(QString& nameAccount, QString& nameDocument){
    std::string uri("file://");
    int accountHashed = this->getDigits(nameAccount.toUtf8().constData());
    int documentHashed = this->getDigits(nameDocument.toUtf8().constData());

    uri = uri + std::to_string(accountHashed) + "/" + std::to_string(documentHashed) + "/" + nameDocument.toUtf8().constData();
    return QString::fromStdString(uri);
}


// if I arrive on this slot, is from a socket that in the
// moment he quits he does not have a open file
void Server::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());
    Account *accountToDisconnect = this->socket2account[signalSender];

    if(accountToDisconnect != nullptr)
        this->socket2account.remove(signalSender);

    //il distruttore lo chiama già la remove oppure no??
    signalSender->deleteLater();
    if(accountToDisconnect != nullptr)
        delete (accountToDisconnect);       //TODO: controlllare il distruttore
}


void Server::onNewConnection() {
    QWebSocket *newSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(newSocket) << " connected!\n";

    //until the account will not be logged in, the value is nullptr
    this->socket2account[newSocket] = nullptr;

    connect(newSocket, &QWebSocket::textMessageReceived, this, &Server::handleNotLoggedRequests);
    connect(newSocket, &QWebSocket::disconnected, this, &Server::disconnectAccount);
}


void Server::removeSocket2AccountPair(QWebSocket *socketOfAccount){
    this->socket2account.remove(socketOfAccount);
}


void Server::removeFile2ServcontrPair(QString filename){
   this->file2serverController.remove(filename);
}


DatabaseManager* Server::getDb(){
    return this->dbOperations;
}








