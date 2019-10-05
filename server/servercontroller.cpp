#include "servercontroller.h"
#include <QThread>

ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{
    m_clients << p_socket;    

    //connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {
    connect(this, &ServerController::messageReceived, this, [&](const QString& p_message) {
        for (QWebSocket *l_client : m_clients) {
            // TODO: restore this
            // if (l_client != pSender) //don't echo message back to sender
            emit messageSent(l_client, p_message);
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
