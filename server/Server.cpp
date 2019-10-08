#include "server.h"

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
    tDocument l_document{new ServerController(), new QThread()};
    l_document.worker->setObjectName("Default doc thread");
    l_document.controller->moveToThread(l_document.worker);
    l_document.worker->start();
    m_documents << l_document;


}

Server::~Server()
{
    m_pWebSocketServer->close();
}


void Server::onNewConnection()
{
    QWebSocket *l_socket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(l_socket) << " connected!\n";
    tDocument l_document = m_documents.at(0);

    //enable messages receiving and forward it to the server controller
    connect(l_socket, &QWebSocket::textMessageReceived,l_document.controller, &ServerController::messageReceived);

    //to delete
    //connect(l_socket,&QWebSocket::textMessageReceived,l_document.controller,&ServerController::messageRec);

    //TODO check queued connection instead of connecting two signal to avoid thread affinity
    connect(l_document.controller, &ServerController::messageSent, this, [&](QWebSocket *p_socket, const QString p_msg){
       p_socket->sendTextMessage(p_msg);
    });

    QMetaObject::invokeMethod(l_document.controller, "addClient",
                              Qt::QueuedConnection, Q_ARG(QWebSocket*, l_socket));
}
