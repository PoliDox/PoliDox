#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QThread>
#include "ServerController.h"
#include "DatabaseManager.h"
#include"ServerMessageFactory.h"



class Server : public QObject
{
    Q_OBJECT

private:
    QWebSocketServer *m_pWebSocketServer;
    QMap<QWebSocket*, Account*> socket2account;
    QMap<QString, ServerController*> file2serverController;
    DatabaseManager *dbOperations;

public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    virtual ~Server();

/* ======================================================================
   The slot is connected to the newConnection signal emitted by the
   QTwebSocket when a new client connects.
   ======================================================================
*/
public slots:
    void onNewConnection();
    void handleNotLoggedRequests(const QString &genericRequestString);
    void handleLoggedRequests(const QString &genericRequestString);
    void disconnectAccount();
};

#endif // SERVER_H
