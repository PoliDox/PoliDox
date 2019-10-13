#include "ServerController.h"
#include "ServerMessageFactory.h"
#include <QThread>


ServerController::ServerController()
{

}

void ServerController::addClient(Account& p_account, QWebSocket *p_socket)
{   
    //qDebug() << "New client (" << p_account.getSiteId() << ")";

    connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {

        qDebug() << "Received a message from a client";
        QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
        for(auto it = m_clients.constBegin(); it != m_clients.constEnd(); it++) {
            const Account& l_client = it.key();
            auto l_socket = it.value();                    
            if (l_socket != pSender) { // don't echo message back to sender                
                l_socket->sendTextMessage(p_message);
            }
        }
    });

    for (auto it = m_clients.constBegin(); it != m_clients.constEnd(); it++) {
        // Notify existing client about the new entry        
        const Account& l_client = it.key();
        auto l_socket = it.value();
        QString l_msg = ServerMessageFactory::createNewClientMessage(p_account);
        l_socket->sendTextMessage(l_msg);

        // Notify new entry about existing client
        l_msg = ServerMessageFactory::createNewClientMessage(l_client);
        p_socket->sendTextMessage(l_msg);
    }

    qDebug() << "Inserting client " << p_account.getSiteId() << " with socket " << p_socket;
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
