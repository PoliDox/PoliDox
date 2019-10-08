#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QThread>
#include "ServerController.h"

struct tDocument {
    ServerController *controller;
    QThread *worker;
};

class Server : public QObject
{
    Q_OBJECT

private:
    QWebSocketServer *m_pWebSocketServer;
    QVector<tDocument> m_documents;

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

};

#endif // SERVER_H
