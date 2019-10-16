#include "Client.h"
#include "Log_dialog.h"
#include "ClientMessageFactory.h"
#include "CrdtClient.h"
#include <QDebug>

Client::Client()
{
    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    // TODO: CHECK IF OPEN SUCCEEDS!

    Log_Dialog loginWindow;
    // loginWindow.setEditor(&m_editor); TODO: this should become set document

    connect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);

    connect(&loginWindow, &Log_Dialog::authDataSubmitted, this, [&](QString p_user, QString p_passw) {
        QByteArray message = ClientMessageFactory::createLoginMessage(p_user, p_passw);
        m_socket.sendTextMessage(message);
    });

    connect(&loginWindow, &Log_Dialog::fileSelected, this, [&](const QString& p_filename) {
        QByteArray message = ClientMessageFactory::createOpenFileMessage(p_filename);
        m_socket.sendTextMessage(message);
    });

    loginWindow.setModal(true);
    loginWindow.exec();


}


void Client::onMessageReceived(const QString &p_msg)
{
    QJsonObject _JSONobj;
    QJsonDocument _JSONdoc;

     _JSONdoc = QJsonDocument::fromJson(p_msg.toUtf8());

    if (_JSONdoc.isNull()) {
        // TODO: print some debug
        return;
    }

    _JSONobj = _JSONdoc.object();
    QString l_header = _JSONobj["action"].toString();

    if (l_header == "loginRepl") {
        QString replCode = _JSONobj["response"].toString();
        // TODO: Check if response is "ok"

        QJsonObject accountObj = _JSONobj["account"].toObject();
        m_user = Account::fromJson(accountObj);
        qDebug() << "Authenticated with siteId" << m_user.getSiteId();

        QJsonArray _JSONfiles=_JSONobj["files"].toArray();
        for(QJsonArray::iterator it=_JSONfiles.begin();it!=_JSONfiles.end();it++){
            m_files.push_back(it->toString());
        }


    } else if (l_header == "openFileRepl") {
        QString replCode = _JSONobj["response"].toString();
        // TODO: Check if response is "ok"

        QJsonObject JSONcrdt = _JSONobj["document"].toObject();
        CrdtClient *crdt = CrdtClient::fromJson(JSONcrdt);
        m_document = new ClientController(crdt, &m_socket);

    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }

}
