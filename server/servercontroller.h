#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>

class ServerController : public QObject
{
public:
    ServerController();

    void addClient(QWebSocket *p_socket);

private:

    QList<QWebSocket *> m_clients;
};

#endif // SERVERCONTROLLER_H
