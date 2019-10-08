#include "ClientController.h"

ClientController::ClientController()
{
    m_crdt = new CRDTclient(this);

    /* takes every character from input and call localInsert */
    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
            QChar car =  m_editor.at(position);
            m_crdt->localInsert(position, car.toLatin1());
        } else {
            //qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            //m_crdt.localDelete(position);
        }
    });


    /* CRDTclient signal onLocalInsert connected to CLIENTcontroller lambda slot.
     * This lamba has to prepare the message that will be sent to the server.
     */
    connect(m_crdt, &CRDTclient::onLocalInsert, this, [&](Char symbol){
        // TODO: create message
        m_socket.sendTextMessage("Message from a client");
    });


    /* SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
     * by server */
    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    connect(&m_socket,&QWebSocket::textMessageReceived, [&](const QString& p_message){
       qDebug() << "Message received:" << p_message;
       //TODO implement remot insert
    });

    m_editor.show();
}

ClientController::~ClientController()
{
    delete m_crdt;
}
