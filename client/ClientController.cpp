#include "ClientController.h"

#include <QLabel>
#include "ClientMessageFactory.h"

ClientController::ClientController()
{
    m_crdt = new CrdtClient(this);

    /* ______________________________________________________________________________________
       takes every character from input and call CRDTclient::localInsert or
       CRDTclient::localDelete.
       //TODO   implementare localDelete.
       ______________________________________________________________________________________      */

    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {

        int _processed=0;

        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
             _processed=charsAdded;
            while(_processed>0){
                char car =  m_editor.at(position).toLatin1();
                if ( 0 == car ) {
                    // Strangely enough \n is given as 0
                    car = '\n';
                }
                m_crdt->localInsert(position, car);
                position++;
                _processed--;
            }

        } else {
            _processed=charsRemoved;
            //qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            while(_processed>0){
                m_crdt->localDelete(position);
                _processed--;
            }
        }
    });


    /* ______________________________________________________________________________________
       CRDTclient signal onLocalInsert connected to CLIENTcontroller lambda slot.
       This lamba has to prepare the message that will be sent to the server:
       //TODO    gestire eventuale fallimento serializzazione
       ______________________________________________________________________________________      */

    connect(m_crdt, &CrdtClient::onLocalInsert, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createInsertMessage(symbol);

        //std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket.sendTextMessage(jsonString);
    });

    connect(m_crdt, &CrdtClient::onLocalDelete, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createDeleteMessage(symbol);

        //std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket.sendTextMessage(jsonString);
    });

    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &ClientController::onTextMessageReceived);

    m_editor.show();    

}


ClientController::~ClientController()
{
    delete m_crdt;
}


/* ______________________________________________________________________________________
   SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
   by server.
   //TODO   prima di richiamare la remoteInsert implementare il non rinvio del messaggio
            al mittente.
   //TODO   avere un booleano all'interno di Char in modo da manetere le azioni sul JSON
            interne alla classe Char?
   ______________________________________________________________________________________     */
void ClientController::onTextMessageReceived(const QString &_JSONstring)
{
    //std::cout<< "Message received" << std::endl;

    QJsonObject _JSONobj;
    QJsonDocument _JSONdoc;    

     _JSONdoc = QJsonDocument::fromJson(_JSONstring.toUtf8());

    if (_JSONdoc.isNull()) {
        // TODO: print some debug
        return;
    }


    _JSONobj = _JSONdoc.object();

    // No try-catch here because we cannot handle it here anyway

    //try {

    // No switch case for strings in C++ :((
    QString l_header = _JSONobj["action"].toString();
    if (l_header == "insert") {
        Char symbol = Char::fromJson(_JSONobj);
        int linPos = m_crdt->remoteInsert(symbol);
        m_editor.remoteInsert(symbol.getSiteId(), linPos, symbol.getValue());

    } else if (l_header== "delete") {
        Char symbol = Char::fromJson(_JSONobj);
        int linPos = m_crdt->remoteDelete(symbol);
        m_editor.remoteDelete(symbol.getSiteId(), linPos);

    } else if (l_header == "newClient") {
        int siteId = _JSONobj["siteId"].toInt();
        m_editor.addClient(siteId);
        qDebug() << "New client with siteId" << siteId;

    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }

    //} catch (std::string _excp) {
        //TODO manage exception
    //}

}