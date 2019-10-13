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


    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setDatabaseName("qtprova");
    db.setUserName("root");     //default xampp  //TODO: da cambiare username
    db.setPassword("");         //default xampp  //TODO: da cambiare password
    bool ok = db.open();
    QTextStream(stdout) << "connessione al db: " << ok << '\n';
}

Server::~Server()
{
    m_pWebSocketServer->close();
}


void Server::onNewConnection()
{
    static int i = 0; // ONLY FOR TESTING

    QWebSocket *l_socket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(l_socket) << " connected!\n";
    Account newUser(i++, "", ""); // TODO: Fill the parameters!
    qDebug() << "Calling addClient";
    m_documents["firstFile"]->addClient(newUser, l_socket);
}
