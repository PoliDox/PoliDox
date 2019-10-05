#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QList>
#include <QWebSocket>

class ServerController : public QObject
{
    Q_OBJECT

private:
    QList<QWebSocket *> m_clients;

public:
    ServerController();

signals:
    void messageReceived(const QString p_msg);

    void messageSent(QWebSocket *p_socket, const QString p_msg);

public slots:
    void addClient(QWebSocket *p_socket);


};

#endif // SERVERCONTROLLER_H
