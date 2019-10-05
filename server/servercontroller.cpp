#include "servercontroller.h"
#include <QThread>

ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{
    m_clients << p_socket;
    qDebug() << "Clients:";
    for (QWebSocket *pClient : m_clients) {
        qDebug() << "client in " << pClient->thread()->objectName();
    }

    //connect(this, &ServerController::messageReceived, this, [&](const QString& p_message) {
    connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {
        qDebug() << "Executing addClient in " << QThread::currentThread()->objectName();
        QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
        for (QWebSocket *pClient : m_clients) {
            if (pClient != pSender) //don't echo message back to sender
                pClient->sendTextMessage(p_message);
        }
    });

    /*
    connect(p_socket, &QWebSocket::disconnected, this, [&](){
        QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
        if (pClient)
        {
            m_clients.removeAll(pClient);
            pClient->deleteLater();
        }
    });
    */
}
