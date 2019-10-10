#include "Server.h"

static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

Server::Server(quint16 port, QObject *parent) :
    QObject(parent)
{
    thread()->setObjectName("Main Thread");

    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Polidox Server"),
                                            QWebSocketServer::NonSecureMode, this);

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "PoliDox server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);

    } else {
        qDebug() << m_pWebSocketServer->errorString();
    }

    // TEST: Initialize the open document list with an empty document
    ServerController *l_firstFile = new ServerController();
    m_documents["firstFile"] = l_firstFile;

}

Server::~Server()
{
    m_pWebSocketServer->close();
}


void Server::onNewConnection()
{
    QWebSocket *l_socket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(l_socket) << " connected!\n";
    m_documents["firstFile"]->addClient(l_socket);
}
