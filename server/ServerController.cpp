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
    //TODO: ricordarsi, al momento opportuno(quando???) di fare la disconnect di questa connect

    this->notifyOtherClients(socketToAdd);

    QList<Account*> accounts;
    for(auto otherSocket : this->socketsOnDocument) {
        if(otherSocket != socketToAdd){
            Account *otherAccount = this->server->getAccount(otherSocket);
            accounts.append(otherAccount);
        }
    }

    QByteArray sendMsgToClient = ServerMessageFactory::createOpenFileReply(true, crdt, accounts);
    socketToAdd->sendTextMessage(sendMsgToClient);

}


// Pay attention that while I notify (.1)the other clients
// about the new_client_connected(== newSocket) I notify also
// (.2)the new_client_connected(me) about all the other clients already connected
void ServerController::notifyOtherClients(QWebSocket *newSocket){
    Account *newAccount = this->server->getAccount(newSocket);
    QString msgNewClient1 = ServerMessageFactory::createNewClientMessage(newAccount);

    for(auto otherSocket : this->socketsOnDocument){
        if(otherSocket != newSocket){            
            otherSocket->sendTextMessage(msgNewClient1);            
        }
    }
}


void ServerController::createCrdt(QList<QString>& orderedInserts){
    this->crdt = new CRDT();
    if(orderedInserts.size() == 0)
        return;

    QJsonArray arrayJson = QJsonArray::fromStringList(orderedInserts);
    this->crdt->fromJson(arrayJson);
}


// The ServerController has to update the his crdt on the
// base of the operation(insert/delete)
void ServerController::handleRemoteOperation(const QString& messageReceivedByClient){
    QJsonObject requestObjJSON;
    QJsonDocument requestDocJSON;

    requestDocJSON = QJsonDocument::fromJson(messageReceivedByClient.toUtf8());
    if (requestDocJSON.isNull()) {
        // TODO: print some debug
        return;
    }
    requestObjJSON = requestDocJSON.object();
    // No switch case for strings in C++ :((
    QString header = requestObjJSON["action"].toString();
    QJsonObject charJson = requestObjJSON["char"].toObject();
    Char charObj = Char::fromJson(charJson);
    QString charValue(charObj.getValue());
    std::vector<int> fractPos(charObj.getFractionalPosition());
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


CRDT* ServerController::getCrdt(){
    return this->crdt;
}










