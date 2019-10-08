#include "ServerController.h"
#include <QThread>

ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{
    m_clients << p_socket;    

    connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {
    //connect(this, &ServerController::messageReceived, this, [&](const QString& p_message) {
        QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
        for (QWebSocket *l_client : m_clients) {
            if (l_client != pSender) { //don't echo message back to sender
                l_client->sendTextMessage(p_message);
            }
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
