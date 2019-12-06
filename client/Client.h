#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "ClientController.h"
#include "Account.h"
#include "LoginWindow.h"
#include <QList>

#define SERVER_IP "127.0.0.1"
//#define SERVER_IP "79.58.200.109"
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
};

#endif // CLIENT_H
