#include "ClientController.h"

ClientController::ClientController()
{
    m_crdt = new CRDTclient(this);

    /* ______________________________________________________________________________________
       takes every character from input and call CRDTclient::localInsert or
       CRDTclient::localDelete.

       //TODO   implementare localDelete.
       ______________________________________________________________________________________      */

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


    /* ______________________________________________________________________________________
       CRDTclient signal onLocalInsert connected to CLIENTcontroller lambda slot.
       This lamba has to prepare the message that will be sent to the server:

       //TODO    gestire eventuale fallimento serializzazione
       ______________________________________________________________________________________      */

    connect(m_crdt, &CRDTclient::onLocalInsert, this, [&](Char symbol){

        QJsonDocument _JSONdoc=symbol.write("insert");

        QString jsonString = _JSONdoc.toJson(QJsonDocument::Indented);

        std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket.sendTextMessage(jsonString);
    });


    /* ______________________________________________________________________________________
       SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
       by server.

       //TODO   prima di richiamare la remoteInsert implementare il non rinvio del messaggio
                al mittente.

       //TODO   avere un booleano all'interno di Char in modo da manetere le azioni sul JSON
                interne alla classe Char?
       ______________________________________________________________________________________     */

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
