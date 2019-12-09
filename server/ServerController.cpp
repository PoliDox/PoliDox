#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>



ServerController::ServerController(QString& p_nameDocumentAssociated, QString& p_uriAssociated, Server *p_server) :
    nameDocumentAssociated(p_nameDocumentAssociated), uriAssociated(p_uriAssociated), server(p_server)
{
    this->crdt = nullptr;
    //nothing else to do???
}


// - We arrive on this slot when a client does a local operation(insert/delete),
//   after that he sends a message to the server via socket,
//   that message has to be duplicated on all the other
//   clients which will do the remote operation
void ServerController::replicateMessageOnOtherSockets(const QString& messageReceivedOnSocket, QWebSocket *signalSender) {
    // The socket of the client which did the local operation
    //QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());

    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;
    requestDocJSON = QJsonDocument::fromJson(messageReceivedOnSocket.toUtf8());
    requestObjJSON = requestDocJSON.object();

    QString header = requestObjJSON["action"].toString();
    if (header == "insert" || header == "delete" || header == "cursorMove"){
        for(auto socket : this->socketsOnDocument){
            if(socket != signalSender)
                socket->sendTextMessage(messageReceivedOnSocket);
        }
    }
}


void ServerController::addClient(QWebSocket *socketToAdd){
    this->socketsOnDocument.push_back(socketToAdd);

    connect(socketToAdd, &QWebSocket::textMessageReceived, this, &ServerController::handleRemoteOperation);
    connect(socketToAdd, &QWebSocket::disconnected, this, &ServerController::disconnectAccount);

    this->notifyOtherClients(socketToAdd);

    //notify the new socket about all the accounts
    //actually connected on the document just opened by him.
    //accountsOnline contains all the OTHER accounts connected
    //on that document except me
    QList<Account*> accountsOnline;
    for(auto otherSocket : this->socketsOnDocument) {
        if(otherSocket != socketToAdd){
            Account *otherAccount = this->server->getAccount(otherSocket);
            accountsOnline.append(otherAccount);
        }
    }

    QString nameDocument = this->nameDocumentAssociated;

    QList<int> allSiteIdsOfDocument;
    QList<Account> allAccountsOfDocument;
    try{
        // otherwise, it is also possible to retrieve directly the accounts
        // objects, without passing through the site ids.
        // we choose this solution for sake of robustness.
        allSiteIdsOfDocument = this->server->getDb()->getAllAccounts(nameDocument);
        allAccountsOfDocument = this->server->getDb()->getAllAccounts(allSiteIdsOfDocument);
    } catch (mongocxx::query_exception) {
        QCoreApplication::removePostedEvents(nullptr);
        QCoreApplication::exit(EXIT_FAILURE);
        return;
    };

    for(Account* acc : accountsOnline){
        allAccountsOfDocument.removeOne(*acc);
    }
    Account* accJustConnected = this->server->getAccount(socketToAdd);
    allAccountsOfDocument.removeOne( *accJustConnected );   //remove also myself from this list
    // now allAccountsOfDocument contains only offline accounts

    QByteArray sendMsgToClient = ServerMessageFactory::createOpenFileReply(true, QString(""), nameDocument, uriAssociated,
                                                                           this->crdt, accountsOnline, allAccountsOfDocument);
    socketToAdd->sendTextMessage(sendMsgToClient);
}


void ServerController::notifyOtherClients(QWebSocket *newSocket){
    Account *newAccount = this->server->getAccount(newSocket);
    QString msgNewClient = ServerMessageFactory::createNewClientMessage(newAccount);

    for(auto otherSocket : this->socketsOnDocument){
        if(otherSocket != newSocket){            
            otherSocket->sendTextMessage(msgNewClient);
        }
    }
}


void ServerController::createCrdt(QList<Char>& orderedInserts){
    this->crdt = new CRDT();

    if(orderedInserts.size() == 0)
        return;

    //QJsonArray arrayJson = QJsonArray::fromStringList(orderedInserts);
    //this->crdt->fromJson(arrayJson);
    this->crdt->fromDatabase(orderedInserts);
}


// The ServerController has to update his crdt on the
// base of the operation(insert/delete)
void ServerController::handleRemoteOperation(const QString& messageReceivedByClient){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());
    QJsonDocument requestDocJSON = QJsonDocument::fromJson(messageReceivedByClient.toUtf8());
    QJsonObject requestObjJSON = requestDocJSON.object();

    QString header = requestObjJSON["action"].toString();
    if (header == "insert") {
        // evaluate if refactor this part
        QJsonObject charJson = requestObjJSON["char"].toObject();
        Char charObj = Char::fromJson(charJson);
        tStyle charStyle = charObj.getStyle();
        QString charValue(charObj.getValue());
        std::vector<int> fractPos(charObj.getFractionalPosition());
        int siteId = charObj.getSiteId();

        this->crdt->remoteInsert(charObj);
        try{
            // evaluate if pass a Char to this function instead of these attributes
            this->server->getDb()->insertSymbol(this->nameDocumentAssociated, charValue, siteId, fractPos,
                                                charStyle.font_family, charStyle.font_size, charStyle.is_bold,
                                                charStyle.is_italic, charStyle.is_underline, charStyle.alignment);
        } catch (mongocxx::bulk_write_exception) {
            QCoreApplication::removePostedEvents(nullptr);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        };

    } else if (header == "delete") {
        // evaluate if refactor this part
        QJsonObject charJson = requestObjJSON["char"].toObject();
        Char charObj = Char::fromJson(charJson);
        tStyle charStyle = charObj.getStyle();   // is it useful?
        QString charValue(charObj.getValue());
        std::vector<int> fractPos(charObj.getFractionalPosition());
        int siteId = charObj.getSiteId();

        this->crdt->remoteDelete(charObj);
        try{
            this->server->getDb()->deleteSymbol(this->nameDocumentAssociated, charValue, siteId, fractPos);
        } catch (mongocxx::bulk_write_exception) {
            QCoreApplication::removePostedEvents(nullptr);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        };

    } else if (header == "cursorMove") {
        // Do Nothing

    } else if(header == "changeImgReq"){
        QString nameAccount = requestObjJSON["nameAccount"].toString();
        QByteArray newImage = requestObjJSON["newImg"].toString().toLatin1();

        try{
            this->server->getDb()->changeImage(nameAccount, newImage);
        } catch (mongocxx::bulk_write_exception) {
            QCoreApplication::removePostedEvents(nullptr);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        };

        QByteArray sendMsgToClient = ServerMessageFactory::createChangeImageReply(true);
        signalSender->sendTextMessage(sendMsgToClient);

    } else if(header == "changePwdReq"){
        QString nameAccount = requestObjJSON["nameAccount"].toString();
        QString newPassword = requestObjJSON["newPwd"].toString();

        try {
            this->server->getDb()->changePassword(nameAccount, newPassword);
        } catch (mongocxx::bulk_write_exception) {
            QCoreApplication::removePostedEvents(nullptr);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        };

        QByteArray sendMsgToClient = ServerMessageFactory::createChangePasswordReply(true);
        signalSender->sendTextMessage(sendMsgToClient);

    } else if (header == "closedEditorReq") {
        bool destroyServContr = false;
        this->socketsOnDocument.removeOne(signalSender);
        if(this->socketsOnDocument.size() == 0){
            destroyServContr = true;
        }

        disconnect(signalSender, &QWebSocket::textMessageReceived, this, &ServerController::handleRemoteOperation);
        disconnect(signalSender, &QWebSocket::disconnected, this, &ServerController::disconnectAccount);
        connect(signalSender, &QWebSocket::textMessageReceived, this->server, &Server::handleLoggedRequests);
        connect(signalSender, &QWebSocket::disconnected, this->server, &Server::disconnectAccount);

        Account* accountQuitted;
        QList<QString> nameDocuments;
        try{
            accountQuitted = this->server->getAccount(signalSender);
            nameDocuments = this->server->getDb()->getAllDocuments(accountQuitted->getSiteId());
        } catch (mongocxx::query_exception) {
            QCoreApplication::removePostedEvents(nullptr);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        };

        QByteArray sendMsgToClientQuitted = ServerMessageFactory::createClosedEditorReply(nameDocuments);
        signalSender->sendTextMessage(sendMsgToClientQuitted);

        if(destroyServContr){
            delete (this);
        } else {
            QByteArray msgForNotifyOtherSockets = ServerMessageFactory::createClosedEditorRemote(accountQuitted);
            for(auto otherSocket : this->socketsOnDocument){
                if(otherSocket != signalSender){
                    otherSocket->sendTextMessage(msgForNotifyOtherSockets);
                }
            }
        }

    } else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }
    this->replicateMessageOnOtherSockets(messageReceivedByClient, signalSender);
}


// if I arrive on this slot, is from a socket that in the
// moment he quits he has an open file
void ServerController::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());

    Account *accountQuitted = this->server->getAccount(signalSender);

    bool destroyServContr = false;
    this->socketsOnDocument.removeOne(signalSender);
    if(this->socketsOnDocument.size() == 0){
        destroyServContr = true;
    }

    this->server->removeSocket2AccountPair(signalSender);

    disconnect(signalSender, &QWebSocket::disconnected, this, &ServerController::disconnectAccount);
    delete (signalSender);

    if(destroyServContr){
        delete (accountQuitted);
        delete (this);
    } else {
        QByteArray msgForNotifyOtherSockets = ServerMessageFactory::createClosedEditorRemote(accountQuitted);
        for(auto otherSocket : this->socketsOnDocument){
            if(otherSocket != signalSender){
                otherSocket->sendTextMessage(msgForNotifyOtherSockets);
            }
        }

        delete (accountQuitted);
    }


}

ServerController::~ServerController(){
    this->server->removeFile2ServcontrPair(this->nameDocumentAssociated);

    delete (this->crdt);
}


CRDT* ServerController::getCrdt(){
    return this->crdt;
}










