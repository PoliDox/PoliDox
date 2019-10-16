#include "Server.h"



static QString getIdentifier(QWebSocket *peer) {
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}


Server::Server(quint16 port, QObject *parent) : QObject(parent) {
    thread()->setObjectName("Main Thread");

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Polidox Server"),
                                              QWebSocketServer::NonSecureMode, this);

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "PoliDox server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);

    } else {
        qDebug() << m_pWebSocketServer->errorString();
    }

    // TEST: Initialize the open document list with an empty document
    ServerController *l_firstFile = new ServerController();
    file2serverController["firstFile"] = l_firstFile;

    //TEST DB
    this->dbOperations = new DatabaseManager();
    //db.registerUser("provauser4","provapsw4","a");
    //std::cout << db.checkPassword("provauser4", "provapsw4") << "\n";
    this->dbOperations->insertNewDocument("provadocument2");

    std::vector<int> v = {7, 5, 16, 8};
    std::vector<int> v2 = {7, 1};
    this->dbOperations->insertSymbol("provadocument", "a", v2);
    //std::cout << db.deleteSymbol("provadocument", "a", v) << "\n";

    this->dbOperations->retrieveAllDocuments();
    for( auto elem : this->dbOperations->retrieveAllDocuments() ){
        std::cout << elem.toUtf8().constData() << "\n";
    }
}


Server::~Server()
{
    m_pWebSocketServer->close();
}


//TODO: - codice copiato da ClientController::onTextMessageReceived, RIFATTORIZZARE!!!
//      - nella creazione dell'account, inserire anche l'immagine
//        quindi checkPassword deve ritornare anche l'immagine oltre che il siteId
//      - cos'è l'action "newClient" ?? sarebbe registrerUser??
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
            //in case of success, result will contain siteId and image(TODO!!)
            loggedAccount = new Account(result, name, ""); //TODO : inserire anche l'immagine
            this->socket2account[signalSender] = loggedAccount;

            //TODO: restituire la lista dei file
            nameDocuments = this->dbOperations->retrieveAllDocuments();

            //TODO: ricordarsi la disconnect e la connect a handleLoggedRequest
            disconnect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleNotLoggedRequests);
            connect(signalSender, &QWebSocket::textMessageReceived, this, &Server::handleLoggedRequests);

            //TODO: ci sono altre cose da fare???
        }

        //TODO: ricontrollare queste due righe!!
        QByteArray sendMsgToClient = ServerMessageFactory::createLoginReply(loginSuccess, loggedAccount, nameDocuments);
        signalSender->sendTextMessage(sendMsgToClient);
    }
    else if (header == "registerUser"){
        QJsonObject accountObj = requestObjJSON["account"].toObject();
        QString password = requestObjJSON["password"].toString();
        Account userToRegister = Account::fromJson(accountObj);

        double result = this->dbOperations->registerUser(userToRegister.getName(),
                                                         password,
                                                         userToRegister.getImage());
        //TODO: ci sono altre cose da fare???

        QByteArray sendMsgToClient = ServerMessageFactory::createRegistrationUserReply(result);
        signalSender->sendTextMessage(sendMsgToClient);

    } else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }

}


//- To handle requests by logged account
//- The only valid actions can be "insert","delete","openFileReq","createFileReq"
void Server::handleLoggedRequests(const QString &genericRequestString){


}


//TODO: - capire bene TUTTE le cose che ci sono da fare
//        quando un acconut di disconnette
void Server::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());

    Account *accountToDisconnet = this->socket2account[signalSender];
    delete (accountToDisconnet);

    //poi ????
}


void Server::onNewConnection() {
    QWebSocket *newSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(newSocket) << " connected!\n";

    //until the account will not be logged in, the value is nullptr
    this->socket2account[newSocket] = nullptr;

    connect(newSocket, &QWebSocket::textMessageReceived, this, &Server::handleNotLoggedRequests);
    connect(newSocket, &QWebSocket::disconnected, this, &Server::disconnectAccount);
}










