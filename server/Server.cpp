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


    //TEST DB

    DatabaseManager db;
    //db.registerUser("provauser4","provapsw4","a");
    //std::cout << db.checkPassword("provauser4", "provapsw4") << "\n";
    db.insertNewDocument("provadocument2");

    std::vector<int> v = {7, 5, 16, 8};
    std::vector<int> v2 = {7, 1};
    db.insertSymbol("provadocument", "a", v2);
    //std::cout << db.deleteSymbol("provadocument", "a", v) << "\n";

    db.retrieveAllInserts("provadocument");
}


Server::~Server()
{
    m_pWebSocketServer->close();
}


//codice login
void Server::onNewConnection()
{
    QWebSocket *l_socket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(l_socket) << " connected!\n";
    m_documents["firstFile"]->addClient(l_socket);
}


//slot per json open new file (namedocument)
//creare o restituire server controller
//controllare file esistente







