#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QThread>
#include "servercontroller.h"

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

public slots:
    void onNewConnection();

};

#endif // SERVER_H
