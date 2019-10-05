#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include "servercontroller.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    virtual ~Server();

private:
    QWebSocketServer *m_pWebSocketServer;
    ServerController m_controller; // TODO: should be a container (multiple files open)

};

#endif // SERVER_H
