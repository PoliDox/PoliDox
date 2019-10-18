#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>
#include <iostream>
#include "Account.h"
#include "Server.h"



class ServerController : public QObject {
    Q_OBJECT

private:
    QList<QWebSocket*> socketsOnDocument;
    QString nameDocumentAssociated;
    Server *server;
    //CRDT c;

public:
    ServerController(QString nameDocumentAssociated, Server *server);
    void addClient(QWebSocket *socketToAdd);
    void notifyOtherClientsAndMe(QWebSocket *newSocket);

public slots:
    void replicateMessageOnOtherSockets(const QString &messageReceivedOnSocket);
    void handleRemoteOperation(const QString &messageReceivedByClient);
};

#endif // SERVERCONTROLLER_H
