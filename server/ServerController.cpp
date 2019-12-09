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
void ServerController::replicateMessageOnOtherSockets(const QString& messageReceivedOnSocket) {
    // The socket of the client which did the local operation
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(QObject::sender());

    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;
    requestDocJSON = QJsonDocument::fromJson(messageReceivedOnSocket.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
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
    //TODO: da sistemare, questa riga qui soto è da togliere: farsi
    //       restituire direttamente gli account senza passare per i siteid
    QList<int> allSiteIdsOfDocument = this->server->getDb()->getAllAccounts(nameDocument);
    QList<Account> allAccountsOfDocument = this->server->getDb()->getAllAccounts(allSiteIdsOfDocument);

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


// The ServerController has to update the his crdt on the
// base of the operation(insert/delete)
void ServerController::handleRemoteOperation(const QString& messageReceivedByClient){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());
    QJsonDocument requestDocJSON = QJsonDocument::fromJson(messageReceivedByClient.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
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

        // evaluate if pass a Char to this function instead of these attributes
        this->server->getDb()->insertSymbol(this->nameDocumentAssociated, charValue, siteId, fractPos,
                                            charStyle.font_family, charStyle.font_size, charStyle.is_bold,
                                            charStyle.is_italic, charStyle.is_underline, charStyle.alignment);
    } else if (header == "delete") {
        // evaluate if refactor this part
        QJsonObject charJson = requestObjJSON["char"].toObject();
        Char charObj = Char::fromJson(charJson);
        tStyle charStyle = charObj.getStyle();   // is it useful?
        QString charValue(charObj.getValue());
        std::vector<int> fractPos(charObj.getFractionalPosition());
        int siteId = charObj.getSiteId();

        this->crdt->remoteDelete(charObj);
        this->server->getDb()->deleteSymbol(this->nameDocumentAssociated, charValue, siteId, fractPos);

    } else if (header == "cursorMove") {
        // Do Nothing

    } else if(header == "changeImgReq"){
        QString nameAccount = requestObjJSON["nameAccount"].toString();
        QByteArray newImage = requestObjJSON["newImg"].toString().toLatin1();

        bool result = this->server->getDb()->changeImage(nameAccount, newImage);

        QByteArray sendMsgToClient = ServerMessageFactory::createChangeImageReply(result);
        signalSender->sendTextMessage(sendMsgToClient);

    } else if(header == "changePwdReq"){
        QString nameAccount = requestObjJSON["nameAccount"].toString();
        QString newPassword = requestObjJSON["newPwd"].toString();

        bool result = this->server->getDb()->changePassword(nameAccount, newPassword);

        QByteArray sendMsgToClient = ServerMessageFactory::createChangePasswordReply(result);
        signalSender->sendTextMessage(sendMsgToClient);

    } else if (header == "closedEditorReq") {
        bool destroyServContr = false;
        this->socketsOnDocument.removeOne(signalSender);
        if(this->socketsOnDocument.size() == 0){
            destroyServContr = true;
            this->server->removeFile2ServcontrPair(this->nameDocumentAssociated);
        }

        disconnect(signalSender, &QWebSocket::textMessageReceived, this, &ServerController::replicateMessageOnOtherSockets);
        disconnect(signalSender, &QWebSocket::textMessageReceived, this, &ServerController::handleRemoteOperation);
        disconnect(signalSender, &QWebSocket::disconnected, this, &ServerController::disconnectAccount);
        connect(signalSender, &QWebSocket::textMessageReceived, this->server, &Server::handleLoggedRequests);
        connect(signalSender, &QWebSocket::disconnected, this->server, &Server::disconnectAccount);

        Account* account = this->server->getAccount(signalSender);
        QList<QString> nameDocuments = this->server->getDb()->getAllDocuments(account->getSiteId());

        QByteArray sendMsgToClientQuitted = ServerMessageFactory::createClosedEditorReply(nameDocuments);
        signalSender->sendTextMessage(sendMsgToClientQuitted);

        Account *accountQuitted = this->server->getAccount(signalSender);
        QByteArray msgForNotifyOtherSockets = ServerMessageFactory::createClosedEditorRemote(accountQuitted);
        for(auto otherSocket : this->socketsOnDocument){
            if(otherSocket != signalSender){
                otherSocket->sendTextMessage(msgForNotifyOtherSockets);
            }
        }

        if(destroyServContr)
            delete (this);
    } else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }
    this->replicateMessageOnOtherSockets(messageReceivedByClient);
}


// if I arrive on this slot, is from a socket that in the
// moment he quits he has an open file
void ServerController::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());

    Account *accountToDisconnect = this->server->getAccount(signalSender);

    bool destroyServContr = false;
    this->socketsOnDocument.removeOne(signalSender);
    if(this->socketsOnDocument.size() == 0){
        destroyServContr = true;
        this->server->removeFile2ServcontrPair(this->nameDocumentAssociated);
    }

    Account *accountQuitted = this->server->getAccount(signalSender);
    QByteArray msgForNotifyOtherSockets = ServerMessageFactory::createClosedEditorRemote(accountQuitted);
    for(auto otherSocket : this->socketsOnDocument){
        if(otherSocket != signalSender){
            otherSocket->sendTextMessage(msgForNotifyOtherSockets);
        }
    }

    this->server->removeSocket2AccountPair(signalSender);

    signalSender->deleteLater();
    delete (accountToDisconnect);
    if(destroyServContr)
        delete (this);
}


CRDT* ServerController::getCrdt(){
    return this->crdt;
}










