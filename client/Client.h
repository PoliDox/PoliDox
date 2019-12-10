#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "ClientController.h"
#include "Account.h"
#include "LoginWindow.h"
#include <QList>

#define SERVER_IP "172.22.117.116"
#define PORT_NUMBER 5678

class Client : public QObject
{
    Q_OBJECT

private:
    ClientController *m_document;
    QWebSocket m_socket;
    Account m_user;
    LoginWindow loginWindow;

public:
    Client();
    ~Client();

public slots:
    void onMessageReceived(const QString& p_msg);
    void onDocClosed();
    void onDocClosedNewFile();
    void onConnectionClosed();
    void onQuit();
};

#endif // CLIENT_H
