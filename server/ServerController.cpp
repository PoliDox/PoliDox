#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>


ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{   
    connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {
    //connect(this, &ServerController::messageReceived, this, [&](const QString& p_message) {
    std::cout << "received a message from a client" << std::endl;
        QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
        for (QWebSocket *l_client : m_clients) {
            if (l_client != pSender) { //don't echo message back to sender
                l_client->sendTextMessage(p_message);
            }
        }
    });

    static int l_siteId = 0; // ONLY FOR TESTING
    l_siteId++;              // ONLY FOR TESTING


    for (QWebSocket *l_client : m_clients) {
        QString l_msg = ServerMessageFactory::createNewClientMessage(l_siteId);
        l_client->sendTextMessage(l_msg);
    }

    m_clients << p_socket;



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
