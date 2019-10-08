#include "servercontroller.h"
#include <QThread>

ServerController::ServerController()
{

}

void ServerController::addClient(QWebSocket *p_socket)
{
    m_clients << p_socket;    

    //connect(p_socket, &QWebSocket::textFrameReceived, this, [&](const QString& p_message) {
    connect(this, &ServerController::messageReceived, this, [&](const QString& _JSONstring) {

        std::cout << "received a message from a client" << std::endl;

        for (QWebSocket *l_client : m_clients) {
            //QTextStream(stdout) << p_message << " connected!\n";

            // TODO: restore this
            // if (l_client != pSender) //don't echo message back to sender
            emit messageSent(l_client, _JSONstring);
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

//to delete
/*void ServerController::messageRec(QString mex){

}*/
