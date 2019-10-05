#include "servercontroller.h"

ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{
    m_clients << p_socket;
    connect(p_socket, &QWebSocket::textMessageReceived, this, [&](const QString& p_message) {
        QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
        for (QWebSocket *pClient : m_clients) {
            if (pClient != pSender) //don't echo message back to sender
                pClient->sendTextMessage(p_message);
        }
    });

    connect(p_socket, &QWebSocket::disconnected, this, [&](){
        QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
        if (pClient)
        {
            m_clients.removeAll(pClient);
            pClient->deleteLater();
        }
    });

}
