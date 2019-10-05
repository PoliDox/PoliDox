#include "server.h"

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

//! [constructor]
Server::Server(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("Chat Server"),
                                            QWebSocketServer::NonSecureMode,
                                            this))
{
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "PoliDox server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, [&](){
            QWebSocket *l_socket = m_pWebSocketServer->nextPendingConnection();
            QTextStream(stdout) << getIdentifier(l_socket) << " connected!\n";
            m_controller.addClient(l_socket);
        });

    } else {
        qDebug() << m_pWebSocketServer->errorString();
    }
}

Server::~Server()
{
    m_pWebSocketServer->close();
}
