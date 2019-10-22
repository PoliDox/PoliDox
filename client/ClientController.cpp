#include "ClientController.h"

#include <QLabel>
#include "ClientMessageFactory.h"

ClientController::ClientController(CrdtClient *p_crdt, QWebSocket *p_socket, QList<Account> accounts) :
    m_crdt(p_crdt), m_socket(p_socket)
{    

    /* ______________________________________________________________________________________
       takes every character from input and call CRDTclient::localInsert or
       CRDTclient::localDelete.
       //TODO   implementare localDelete.
       ______________________________________________________________________________________      */

    connect(&m_editor, &Editor::textChanged, this, [&](int position, int charsRemoved, int charsAdded) {

        // TODO: Test what happens when we replace some text with other text (Select some text and Ctrl-V)
        // Probably we should delete everything first and then insert..

        qDebug() << charsAdded << " chars added and " << charsRemoved << " chars removed at position " << position;

        if (charsAdded > 1 && position == 0 &&
                m_editor.at(0) != QChar::ParagraphSeparator) {
            // When copying to the beginning everything is deleted and copied anew
            charsAdded--;
            if (!charsRemoved) {
                //qWarning() << "ATTENTION: the assumption at the beginning of this block is wrong!";
            }
            charsRemoved--;
        }

        // It could happen that some chars were removed and some others were added at the same time
        for (int i = 0; i < charsRemoved; i++) {
            m_crdt->localDelete(position);
        }

        for (int i = 0; i < charsAdded; i++) {

            QChar qchar = m_editor.at(position+i);
            char _char;
            if (qchar == QChar::ParagraphSeparator) {
                qDebug() << "ParagraphSeparator";
                _char = '\n';
            } else {
                _char =  qchar.toLatin1();
            }
            m_crdt->localInsert(position+i, _char);
        }

    });


    for (Account ac : accounts) {
        m_editor.addClient(ac);
    }

    /* ______________________________________________________________________________________
       CRDTclient signal onLocalInsert connected to CLIENTcontroller lambda slot.
       This lamba has to prepare the message that will be sent to the server:
       //TODO    gestire eventuale fallimento serializzazione
       ______________________________________________________________________________________      */

    connect(m_crdt, &CrdtClient::onLocalInsert, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createInsertMessage(symbol);

        //std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket->sendTextMessage(jsonString);
    });

    connect(m_crdt, &CrdtClient::onLocalDelete, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createDeleteMessage(symbol);

        //std::cout << jsonString.toUtf8().constData() <<std::endl;

        m_socket->sendTextMessage(jsonString);
    });

    connect(m_socket, &QWebSocket::textMessageReceived, this, &ClientController::onTextMessageReceived);

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
        QJsonObject charObj = _JSONobj["char"].toObject();
        Char symbol = Char::fromJson(charObj);
        int linPos = m_crdt->remoteInsert(symbol);
        m_editor.remoteInsert(symbol.getSiteId(), linPos, symbol.getValue());

    } else if (l_header== "delete") {
        QJsonObject charObj = _JSONobj["char"].toObject();
        Char symbol = Char::fromJson(charObj);
        int linPos = m_crdt->remoteDelete(symbol);
        m_editor.remoteDelete(symbol.getSiteId(), linPos);

    } else if (l_header == "newClient") {
        QJsonObject accountObj = _JSONobj["account"].toObject();
        Account newUser = Account::fromJson(accountObj);
        m_editor.addClient(newUser);
        qDebug() << "New client with siteId" << newUser.getSiteId();

    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }

    //} catch (std::string _excp) {
        //TODO manage exception
    //}

}
