#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>



ServerController::ServerController(QString &nameDocumentAssociated, Server *server) {
    this->nameDocumentAssociated = nameDocumentAssociated;
    this->server = server;
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

    for(auto socket : this->socketsOnDocument){
        if(socket != signalSender)
            socket->sendTextMessage(messageReceivedOnSocket);
    }
}


void ServerController::addClient(QWebSocket *socketToAdd){
    this->socketsOnDocument.push_back(socketToAdd);

    connect(socketToAdd, &QWebSocket::textMessageReceived, this, &ServerController::replicateMessageOnOtherSockets);
    connect(socketToAdd, &QWebSocket::textMessageReceived, this, &ServerController::handleRemoteOperation);
    connect(socketToAdd, &QWebSocket::disconnected, this, &ServerController::disconnectAccount);
    //TODO: ricordarsi, al momento opportuno(quando???) di fare la disconnect di questa connect

    this->notifyOtherClients(socketToAdd);

    //notify the new socket about all the accounts
    //actually connected on the document just opened by him
    QList<Account*> accounts;
    for(auto otherSocket : this->socketsOnDocument) {
        if(otherSocket != socketToAdd){
            Account *otherAccount = this->server->getAccount(otherSocket);
            accounts.append(otherAccount);
        }
    }

    QByteArray sendMsgToClient = ServerMessageFactory::createOpenFileReply(true, this->crdt, accounts);
    //qDebug() << "sendMsgToClient:  " << QString(sendMsgToClient).toUtf8().constData();
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
    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;

    qDebug() << "Handling remote operation: " << messageReceivedByClient.toUtf8().constData();

    requestDocJSON = QJsonDocument::fromJson(messageReceivedByClient.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
    requestObjJSON = requestDocJSON.object();

    QJsonObject charJson = requestObjJSON["char"].toObject();
    Char charObj = Char::fromJson(charJson);
    qDebug() << "value: " << charObj.getValue();
    qDebug() << "position: " << charObj.getFractionalPosition();
    QString charValue(charObj.getValue());
    std::vector<int> fractPos(charObj.getFractionalPosition());

    QString header = requestObjJSON["action"].toString();
    if (header == "insert") {
        this->crdt->remoteInsert(charObj);
        this->server->getDb()->insertSymbol(this->nameDocumentAssociated, charValue, fractPos);
    }
    else if(header == "delete"){
        this->crdt->remoteDelete(charObj);
        this->server->getDb()->deleteSymbol(this->nameDocumentAssociated, charValue, fractPos);
    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }

}


// if I arrive on this slot, is from a socket that in the
// moment he quits he has an open file
void ServerController::disconnectAccount(){
    QWebSocket *signalSender = qobject_cast<QWebSocket *>(sender());

    Account *accountToDisconnect = this->server->getAccount(signalSender);

    this->server->removeSocketAccountPair(signalSender);

    bool destroyServContr = false;
    this->socketsOnDocument.removeOne(signalSender);
    if(this->socketsOnDocument.size() == 0){
        destroyServContr = true;
        this->server->removeFileServcontrPair(this->nameDocumentAssociated);
    }

    //il distruttore lo chiama già la remove oppure no??
    signalSender->deleteLater();
    delete (accountToDisconnect);       //TODO: controlllare il distruttore
    if(destroyServContr)
        delete (this);                  //TODO: controlllare il distruttore
}


CRDT* ServerController::getCrdt(){
    return this->crdt;
}










