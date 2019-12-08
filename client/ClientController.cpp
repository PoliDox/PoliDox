#include "ClientController.h"

#include <QLabel>
#include <QMessageBox>
#include "ClientMessageFactory.h"


ClientController::ClientController(QWebSocket *p_socket, const Account& p_account, const QString& p_fileName, const QString& p_uri, QList<Account>& contributorsOnline, QList<Account>& contributorsOffline) :
    m_socket(p_socket), m_account(p_account), m_filename(p_fileName), m_uri(p_uri)
{        
    m_siteId = p_account.getSiteId();
    m_crdt = new CrdtClient(m_siteId);
    m_editor = new Editor(this, nullptr, contributorsOnline, contributorsOffline, &p_account);

    connect(m_editor, &Editor::textChanged, this, &ClientController::onTextChanged);

    connect(m_crdt, &CrdtClient::onLocalInsert, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createInsertMessage(symbol, m_siteId);
        //qDebug() << "Sending local insert: " << QString(jsonString).toUtf8().constData();
        m_socket->sendTextMessage(jsonString);
    });

    connect(m_crdt, &CrdtClient::onLocalDelete, this, [&](Char symbol){

        QByteArray jsonString = ClientMessageFactory::createDeleteMessage(symbol, m_siteId);
        //std::cout << jsonString.toUtf8().constData() <<std::endl;
        m_socket->sendTextMessage(jsonString);
    });

    connect(m_editor, &Editor::cursorPositionChanged, this, [&](int p_pos) {
        QByteArray jsonString = ClientMessageFactory::createCursorMoveMessage(p_pos, m_siteId);
        m_socket->sendTextMessage(jsonString);
    });

    connect(m_socket, &QWebSocket::textMessageReceived, this, &ClientController::onTextMessageReceived);

    connect(m_editor, &Editor::quit_editor, this, &ClientController::docClosed);

    connect(m_editor, &Editor::quit_editor_new_file, this, &ClientController::docClosedNewFile);

    connect(m_editor, &Editor::ChangeImgEditor, this, [&](QPixmap Pix){
        QByteArray jsonString = ClientMessageFactory::createImgUpdate(p_account.getName(), Pix);
        this->Pix = Pix;
        m_socket->sendTextMessage(jsonString);
    });

    connect(m_editor, &Editor::ChangePwdEditor, this, [&](QString Pwd){
        QByteArray jsonString = ClientMessageFactory::createPwdUpdate(p_account.getName(), Pwd);
        m_socket->sendTextMessage(jsonString);
    });

    m_editor->show();
}


ClientController::~ClientController()
{
    delete m_crdt;
    delete m_editor;
}


//TODO: da ottimizzare, evire le copie, soprattutto sui vettori
void ClientController::init(const QJsonArray& p_crdt) {

    std::vector<std::vector<Char>> symbolsOfOpenedDocument = CRDT::fromJson(p_crdt);
    this->m_crdt->setSymbols(symbolsOfOpenedDocument);

    QVector<Char> text;

    for(std::vector<Char> elem : this->m_crdt->getSymbols())
        for(Char symbol : elem)
            text += symbol;

    m_editor->init(text);
}

QVector<int> ClientController::getUserChars(int p_siteId)
{
    std::vector<int> userChars = m_crdt->getUserPositions(p_siteId);
    return QVector<int>::fromStdVector(userChars);
}


/* ______________________________________________________________________________________
   SOCKETsignal connected to CLIENTcontroller lambda in order to catch messages forwarded
   by server.
   ______________________________________________________________________________________ */
void ClientController::onTextMessageReceived(const QString &_JSONstring)
{
    //std::cout<< "Message received" << std::endl;

    QJsonObject _JSONobj;
    QJsonDocument _JSONdoc;    

     _JSONdoc = QJsonDocument::fromJson(_JSONstring.toUtf8());

    if (_JSONdoc.isNull()) {
        qWarning() << "Json is NULL";
        return;
    }

    _JSONobj = _JSONdoc.object();

    QString l_header = _JSONobj["action"].toString();
    if (l_header == "insert") {
        //qDebug() << "REMOTE INSERT:" << _JSONstring.toUtf8().constData();
        QJsonObject charObj = _JSONobj["char"].toObject();
        Char symbol = Char::fromJson(charObj);
        int linPos = m_crdt->remoteInsert(symbol);
        int siteId = _JSONobj["siteId"].toInt();
        m_editor->handleRemoteOperation(INSERT_OP, symbol, linPos, siteId);

    } else if (l_header== "delete") {
        QJsonObject charObj = _JSONobj["char"].toObject();
        Char symbol = Char::fromJson(charObj);
        int linPos = m_crdt->remoteDelete(symbol);
        int siteId = _JSONobj["siteId"].toInt();
        m_editor->handleRemoteOperation(DELETE_OP, symbol, linPos, siteId);

    } else if (l_header == "cursorMove") {
        int pos = _JSONobj["position"].toInt();
        int siteId = _JSONobj["siteId"].toInt();
        m_editor->moveCursor(pos, siteId);

    } else if (l_header == "newClient") {
        QJsonObject accountObj = _JSONobj["account"].toObject();
        Account newUser = Account::fromJson(accountObj);
        m_editor->addClient(newUser);        
        //qDebug() << "New client with siteId" << newUser.getSiteId();

    } else if (l_header == "closedEditorRemote") {
        QJsonObject accountObj = _JSONobj["account"].toObject();
        Account offlineUser = Account::fromJson(accountObj);
        m_editor->removeClient(offlineUser);

    } else if (l_header == "changePwdRepl") {
        QString response = _JSONobj["response"].toString();
        if ( response == "ok" ) {
            QMessageBox::information(m_editor->getProfilePtr(), "PoliDox", "Password correctly updated");
        } else {
            QMessageBox::warning(m_editor->getProfilePtr(), "PoliDox", "Password update failed");
        }
    } else if (l_header == "changeImgRepl") {
        QString response = _JSONobj["response"].toString();
        if ( response == "ok") {
            m_account.setImage(Pix);
            m_editor->setNewImage(Pix);
            QMessageBox::information(m_editor->getProfilePtr(), "PoliDox", "Image correctly updated");
            m_editor->getProfilePtr()->raise();
        } else {
            QMessageBox::warning(m_editor->getProfilePtr(), "PoliDox", "Image update failed");
        }
    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }   

}

void ClientController::onTextChanged(int position, int charsRemoved, int charsAdded)
{

    qDebug() << "Chars added: " << charsAdded << ", chars removed: " << charsRemoved;    

    /*
    if (charsAdded > 1 && position == 0 &&
            m_editor->at(0) != QChar::ParagraphSeparator) {
        // When copying to the beginning everything is deleted and copied anew
        charsAdded--;
        if (!charsRemoved) {
            //qWarning() << "ATTENTION: the assumption at the beginning of this block is wrong!";
        }
        charsRemoved--;
    }
    */
    if (charsAdded > m_editor->textSize()) {
        qDebug() << "Adjusting chars added and removed";
        charsAdded--;
        charsRemoved--;
    }

    // It could happen that some chars were removed and some others were added at the same time
    for (int i = 0; i < charsRemoved; i++) {
        m_crdt->localDelete(position);

    }

    for (int i = 0; i < charsAdded; i++) {
        /************************************************************************************************/
        //This is needed to avoid that the character inserted copies the background of the previous character
        if(i==0){
            QColor color=m_editor->userSelected(m_crdt->getSiteId());
            m_editor->resetBackgroundColor(position,color);
        }
        /************************************************************************************************/
        QChar qchar = m_editor->at(position+i);
        ushort _char;
        if (qchar == QChar::ParagraphSeparator) {
            //qDebug() << "ParagraphSeparator";
            _char = '\n';
        } else {
            _char =  qchar.unicode();
        }

        Char symbol(_char,m_crdt->getSiteId());

        m_editor->setCharacterStyle(position+i+1,symbol); //Set the character style before forwarding it to local insert
        m_crdt->localInsert(position+i, symbol);
    }

}

