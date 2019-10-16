#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>
#include <iostream>
#include "Account.h"



class ServerController : public QObject {
    Q_OBJECT

private:
    //TODO: in realtà questa mappa andrebbe al contrario,
    //io dalle slot sono in grado di risalire al socket,
    //e quindi poi all'account
    QMap<Account, QWebSocket*> account2socket;
    //CRDT c;

public:
    ServerController();

public slots:
    void addClient(Account& p_account, QWebSocket *p_socket);

};

#endif // SERVERCONTROLLER_H
