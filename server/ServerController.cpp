#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>


ServerController::ServerController()
{

}

void ServerController::addClient(Account& p_account, QWebSocket *p_socket)
{   
    qDebug() << "addClient called";

    connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {

        qDebug() << "Received a message from a client";

        for(auto it = m_clients.constBegin(); it != m_clients.constEnd(); it++) {
            auto l_socket = it.value();
            if (l_socket != p_socket) { //don't echo message back to sender
                l_socket->sendTextMessage(p_message);
            }
        }
    });

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); it++) {
        // Notify existing client about the new entry        
        const Account& l_client = it.key();
        auto l_socket = it.value();
        qDebug() << "Notifying client " << l_client.getSiteId();
        QString l_msg = ServerMessageFactory::createNewClientMessage(p_account);
        l_socket->sendTextMessage(l_msg);

        // Notify new entry about existing client
        l_msg = ServerMessageFactory::createNewClientMessage(l_client);
        p_socket->sendTextMessage(l_msg);
    }

    qDebug() << "Inserting client, already got " << m_clients.size();
    m_clients.insert(p_account, p_socket);    


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
