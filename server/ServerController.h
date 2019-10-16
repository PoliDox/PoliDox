#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>
#include <iostream>
#include "Account.h"



class ServerController : public QObject
{
    Q_OBJECT

private:
    QMap<Account, QWebSocket*> m_clients;

public:
    ServerController();

public slots:
    void addClient(Account& p_account, QWebSocket *p_socket);

};

#endif // SERVERCONTROLLER_H
