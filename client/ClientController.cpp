#include "ClientController.h"

ClientController::ClientController()
{
    m_crdt = new CRDTclient(this);

    /* ______________________________________________________________________________________
       takes every character from input and call CRDTclient::localInsert or
       CRDTclient::localDelete
       ______________________________________________________________________________________      */

    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
            QChar car =  m_editor.at(position);
            m_crdt->localInsert(position, car.toLatin1());
        } else {
            //qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            //m_crdt.localDelete(position); //TODO implement local delete
        }
    });


    /* ______________________________________________________________________________________
       CRDTclient signal onLocalInsert connected to CLIENTcontroller lambda slot.
       This lamba has to prepare the message that will be sent to the server.
       ______________________________________________________________________________________      */

    connect(m_crdt, &CRDTclient::onLocalInsert, this, [&](Char symbol){
        // TODO: create message
        QJsonDocument _JSONdoc=symbol.write("insert");

        QString jsonString = _JSONdoc.toJson(QJsonDocument::Indented);

        std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket.sendTextMessage(jsonString);
    });


    /* ______________________________________________________________________________________
       SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
       by server
       ______________________________________________________________________________________     */

    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    connect(&m_socket,&QWebSocket::textMessageReceived, [&](const QString& _JSONstring){
       qDebug() << "Message received:";

       //TODO Here the message sent from the server must be unmarshalled and a symbol must be create
       Char symbol=Char::read(_JSONstring);

       //this->m_crdt->remoteInsert(symbol);

    });

    m_editor.show();
}


ClientController::~ClientController()
{
    delete m_crdt;
}
