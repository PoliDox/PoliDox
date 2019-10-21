#include "Client.h"
#include "ClientMessageFactory.h"
#include "CrdtClient.h"
#include <QDebug>

Client::Client()
{
    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));
    // TODO: CHECK IF OPEN SUCCEEDS!


    connect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);

    connect(&loginWindow, &Log_Dialog::authDataSubmitted, this, [&](QString p_user, QString p_passw) {
        QByteArray message = ClientMessageFactory::createLoginMessage(p_user, p_passw);
        m_socket.sendTextMessage(message);
    });

    connect(&loginWindow, &Log_Dialog::signupDataSubmitted, this, [&](QString p_user, QString p_passw) {
        QByteArray message = ClientMessageFactory::createRegisterMessage(p_user, p_passw);
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

        if (replCode != "ok") {
            qDebug() << "Authentication failed";
            return;
        }

        QJsonObject accountObj = _JSONobj["account"].toObject();
        m_user = Account::fromJson(accountObj);
        qDebug() << "Authenticated with siteId" << m_user.getSiteId();

        QJsonArray _JSONfiles=_JSONobj["files"].toArray();
        for(QJsonArray::iterator it=_JSONfiles.begin();it!=_JSONfiles.end();it++){
            m_files.push_back(it->toString());
        }
        loginWindow.displayFiles(m_files);

    } else if (l_header == "registerUserRepl") {
        QString replCode = _JSONobj["response"].toString();
        if (replCode != "ok") {
            qDebug() << "Registration failed";
            return;
        }


    } else if (l_header == "openFileRepl") {
        QString replCode = _JSONobj["response"].toString();
        if (replCode != "ok") {
            qDebug() << "Couldn't open file";
            return;
        }

        QJsonArray JSONcrdt = _JSONobj["document"].toArray();
        CrdtClient *crdt = new CrdtClient(-1);      //TODO: impostare qui il siteId corretto(si trova nella json reply)
        crdt->fromJson(JSONcrdt);
        m_document = new ClientController(crdt, &m_socket);

    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }

}
