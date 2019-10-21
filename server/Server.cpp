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

    // TEST: Initialize the open document list with an empty document
    //ServerController *l_firstFile = new ServerController();
    //file2serverController["firstFile"] = l_firstFile;

    //TEST DB
    this->dbOperations->registerUser("provauser5","provapsw5","a");
    //std::cout << db.checkPassword("provauser4", "provapsw4") << "\n";
    //this->dbOperations->insertNewDocument("provadocument2");

    //std::vector<int> v = {7, 5, 16, 8};
    //std::vector<int> v2 = {7, 1};
    //this->dbOperations->insertSymbol("provadocument", "a", v2);
    //std::cout << db.deleteSymbol("provadocument", "a", v) << "\n";

    //this->dbOperations->retrieveAllDocuments();
    //for( auto elem : this->dbOperations->retrieveAllDocuments() ){
    //    std::cout << elem.toUtf8().constData() << "\n";
    //}

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
void Server::handleNotLoggedRequests(const QString &genericRequestString){
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
        double result = this->dbOperations->checkPassword(name, password);
        if(result >= 0){
            loginSuccess = true;
            //in case of success, result will contain siteId and [image(TODO!!)]
            loggedAccount = new Account(result, name, ""); //TODO : inserire anche l'immagine
            this->socket2account[signalSender] = loggedAccount;

            nameDocuments = this->dbOperations->getAllDocuments();

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
        double siteId = this->dbOperations->registerUser(name, password, image);
        if(siteId >= 0)
            registrationSuccess = true;

        QByteArray sendMsgToClient = ServerMessageFactory::createRegistrationUserReply(registrationSuccess, siteId);
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }

}


ServerController* Server::initializeServerController(QString nameDocument, QList<QString> orderedInserts){
    ServerController *fileServContr = new ServerController(nameDocument, this);
    fileServContr->createCrdt(orderedInserts);
    return fileServContr;
}


//- To handle requests by logged account
//- The only valid actions can be "openFileReq","createFileReq"
void Server::handleLoggedRequests(const QString &genericRequestString){
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
            QList<QString> orderedInserts = this->dbOperations->getAllInserts(nameDocument);
            fileServContr = this->initializeServerController(nameDocument, orderedInserts);
        }
        this->file2serverController[nameDocument] = fileServContr;

        //TODO: gestire la open file reply nel caso in cui la richiesta
        //      non vada a buon fine

        fileServContr->addClient(signalSender);
        disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);

        //da sistemare
        QByteArray sendMsgToClient = ServerMessageFactory::createOpenFileReply(true, fileServContr->getCrdt());
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else if (header == "createFileReq"){
        //create and open the file
        double result = this->dbOperations->insertNewDocument(nameDocument);
        if(result){
            QList<QString> emptyList;
            fileServContr = this->initializeServerController(nameDocument, emptyList);
            this->file2serverController[nameDocument] = fileServContr;

            fileServContr->addClient(signalSender);
            disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);

            //TODO: per ora risponde con una openfilereply anziché
            //      con una createCreateFileReply
            QByteArray sendMsgToClient = ServerMessageFactory::createOpenFileReply(true, fileServContr->getCrdt());
            signalSender->sendTextMessage(sendMsgToClient);
        } else {
            //TODO: gestire la reply in caso di fallimento
        }

    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }
}


//TODO: - capire bene TUTTE le cose che ci sono da fare
//        quando un acconut di disconnette
void Server::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());

    Account *accountToDisconnet = this->socket2account[signalSender];
    delete (accountToDisconnet);

    //TODO: la delete dell'oggetto socket presente(come chiave)
    //      nella mappa va fatta oppure no ???

    this->socket2account.remove(signalSender);

    //poi ????
}


void Server::onNewConnection() {
    QWebSocket *newSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(newSocket) << " connected!\n";

    //until the account will not be logged in, the value is nullptr
    this->socket2account[newSocket] = nullptr;

    connect(newSocket, &QWebSocket::textMessageReceived, this, &Server::handleNotLoggedRequests);
    connect(newSocket, &QWebSocket::disconnected, this, &Server::disconnectAccount);    //TODO: da rivedere
}


DatabaseManager* Server::getDb(){
    return this->dbOperations;
}








