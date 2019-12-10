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
    QString uriAssociated;
    Server *server;
    CRDT *crdt;

public:
    ServerController(QString &p_nameDocumentAssociated, QString &p_uriAssociated, Server *p_server);
    ~ServerController();
    void addClient(QWebSocket *socketToAdd);
    void notifyOtherClients(QWebSocket *newSocket);
    void createCrdt(QList<Char>& orderedInserts);
    CRDT* getCrdt();
    ServerController(const ServerController& other) = delete;
    ServerController(const ServerController&& other) = delete;

    void replicateMessageOnOtherSockets(const QString& messageReceivedOnSocket, QWebSocket *signalSender);

public slots:
    void handleRemoteOperation(const QString& messageReceivedByClient);
    void disconnectAccount();
};

#endif // SERVERCONTROLLER_H
