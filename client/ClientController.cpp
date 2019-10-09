#include "ClientController.h"

ClientController::ClientController()
{
    m_crdt = new CrdtClient(this);

    /* takes every character from input and call localInsert */
    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
            QChar car =  m_editor.at(position);
            m_crdt->localInsert(position, car.toLatin1());
        } else {

            qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            m_crdt->localDelete(position); //TODO implement local delete
        }
    });


    /* CrdtClient signal onLocalInsert connected to CLIENTcontroller lambda slot.
     * This lamba has to prepare the message that will be sent to the server.
     */
    connect(m_crdt, &CrdtClient::onLocalInsert, this, [&](Char symbol){
        // TODO: create message
        m_socket.sendTextMessage("Message from a client");
    });


    /* SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
     * by server */
    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));

    connect(&m_socket,&QWebSocket::textMessageReceived, [&](const QString& _JSONstring){

       std::cout<< "Message received:"<<std::endl;

       QJsonObject _JSONobj;
       QJsonDocument _JSONdoc;

       Char symbol=Char::read(_JSONstring);

        _JSONdoc=QJsonDocument::fromJson(_JSONstring.toUtf8());

       if(!_JSONdoc.isNull())
            _JSONobj=_JSONdoc.object();

       if(_JSONobj["action"].toString()=="insert")
               this->m_crdt->remoteInsert(symbol);

       if(_JSONobj["action"].toString()=="delete")
               this->m_crdt->remoteDelete(symbol);



    });

    m_editor.show();
}

ClientController::~ClientController()
{
    delete m_crdt;
}
