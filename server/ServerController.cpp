#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>



ServerController::ServerController(QString nameDocumentAssociated, Server *server) {
    this->nameDocumentAssociated = nameDocumentAssociated;
    this->server = server;
    this->crdt = nullptr;
    //nothing else to do???
}


// - We arrive on this slot when a client does a local operation(insert/delete),
//   after that he sends a message to the server via socket,
//   that message has to be duplicated on all the other
//   clients which will do the remote operation
void ServerController::replicateMessageOnOtherSockets(const QString &messageReceivedOnSocket) {
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

    this->notifyOtherClientsAndMe(socketToAdd);
}


// Pay attention that while I notify (.1)the other clients
// about the new_client_connected(== newSocket) I notify also
// (.2)the new_client_connected(me) about all the other clients already connected
void ServerController::notifyOtherClientsAndMe(QWebSocket *newSocket){
    Account *newAccount = this->server->getAccount(newSocket);
    QString msgNewClient1 = ServerMessageFactory::createNewClientMessage(newAccount);

    for(auto otherSocket : this->socketsOnDocument){
        if(otherSocket != newSocket){
            //(.1) notify other client
            otherSocket->sendTextMessage(msgNewClient1);

            //(.2) notify me
            Account *otherAccount = this->server->getAccount(otherSocket);
            QString msgNewClient2 = ServerMessageFactory::createNewClientMessage(otherAccount);
            newSocket->sendTextMessage(msgNewClient2);
        }
    }
}


void ServerController::createCrdt(QList<QString> orderedInserts){
    if(orderedInserts.size() == 0){
        this->crdt = new CRDT();
        return;
    }

    QJsonArray arrayJson = QJsonArray::fromStringList(orderedInserts);
    this->crdt = this->crdt->fromJson(arrayJson);
}


// The ServerController has to update the his crdt on the
// base of the operation(insert/delete)
void ServerController::handleRemoteOperation(const QString &messageReceivedByClient){
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
    if (header == "insert") {
        this->crdt->remoteInsert(charObj);
        this->server->getDb()->insertSymbol(this->nameDocumentAssociated, QString(charObj.getValue()),
                                            charObj.getFractionalPosition());
    }
    else if(header == "delete"){
        this->crdt->remoteDelete(charObj);
        this->server->getDb()->deleteSymbol(this->nameDocumentAssociated, QString(charObj.getValue()),
                                            charObj.getFractionalPosition());
    }
    else {
        qWarning() << "Unknown message received: " << requestObjJSON["action"].toString();
    }

}


CRDT* ServerController::getCrdt(){
    return this->crdt;
}










