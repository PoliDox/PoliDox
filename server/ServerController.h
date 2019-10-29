#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>
#include <iostream>
#include "Account.h"
#include "Server.h"
#include "CRDT.h"



class ServerController : public QObject {
    Q_OBJECT

private:
    QList<QWebSocket*> socketsOnDocument;
    QString nameDocumentAssociated;
    Server *server;
    CRDT *crdt;

public:
    ServerController(QString &nameDocumentAssociated, Server *server);
    void addClient(QWebSocket *socketToAdd);
    void notifyOtherClients(QWebSocket *newSocket);
    void createCrdt(QList<Char>& orderedInserts);
    CRDT* getCrdt();

public slots:
    void replicateMessageOnOtherSockets(const QString& messageReceivedOnSocket);
    void handleRemoteOperation(const QString& messageReceivedByClient);
    void disconnectAccount();
};

#endif // SERVERCONTROLLER_H
