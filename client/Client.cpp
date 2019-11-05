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
        // TODO: delete this, c_fileName should be filled at openFileRepl
        c_fileName = p_filename;
    });

    connect(&loginWindow, &Log_Dialog::uriSelected, this, [&](const QString& p_uri) {
        QByteArray message = ClientMessageFactory::createOpenFileMessage(QString(), p_uri);
        qDebug() << QString(message).toUtf8().constData();
        m_socket.sendTextMessage(message);
    });

    connect(&loginWindow, &Log_Dialog::newFileSelected, this, [&](const QString& p_filename) {
        QByteArray message = ClientMessageFactory::createNewFileMessage(p_filename);
        m_socket.sendTextMessage(message);
        c_fileName = p_filename;
    });

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
        int r, g, b;
        m_user.getColor().getRgb(&r, &g, &b);
        qDebug() << "Color " << r << ", " << g << ", " << b;

        QJsonArray _JSONfiles=_JSONobj["nameDocuments"].toArray();
        QList<QString> l_files;
        for(QJsonArray::iterator it=_JSONfiles.begin();it!=_JSONfiles.end();it++){
            l_files.push_back(it->toString());
        }
        loginWindow.displayFiles(l_files);

    } else if (l_header == "registerUserRepl") {
        QString replCode = _JSONobj["response"].toString();
        if (replCode != "ok") {
            qDebug() << "Registration failed";
            return;
        }


    } else if (l_header == "openFileRepl" || l_header == "createFileRepl") {
        QString replCode = _JSONobj["response"].toString();
        if (replCode != "ok") {
            qDebug() << "Couldn't open file";
            return;
        }
        //std::cout << "JSON ARRIVATO:" << p_msg.toUtf8().constData() << "\n\n\n";
        QJsonArray JSONcrdt = _JSONobj["crdt"].toArray();
        QJsonArray JSONaccounts = _JSONobj["accounts"].toArray();
        QJsonArray JSONaccountsOffline = _JSONobj["accountsOffline"].toArray();

        QList<Account> contributorsOnline;
        QList<Account> contributorsOffline;

        if (l_header == "openFileRepl") {
            for (const QJsonValue accOnline : JSONaccounts)
                contributorsOnline.push_back(Account::fromJson(accOnline.toObject()));
            for (const QJsonValue accOffline : JSONaccountsOffline)
                contributorsOffline.push_back(Account::fromJson(accOffline.toObject()));
        }
        m_document = new ClientController(&m_socket, m_user.getSiteId(), c_fileName, contributorsOnline, contributorsOffline);
        m_document->init(JSONcrdt, JSONaccounts);

        loginWindow.hide();
        disconnect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);

        connect(m_document, &ClientController::docClosed, this, &Client::onDocClosed);

    } else if (l_header == "closedEditorRepl") {
        QJsonArray _JSONfiles=_JSONobj["nameDocuments"].toArray();
        QList<QString> l_files;
        for(QJsonArray::iterator it=_JSONfiles.begin();it!=_JSONfiles.end();it++){
            l_files.push_back(it->toString());
        }
        loginWindow.displayFiles(l_files);

    } else {
        qWarning() << "Unknown message received: " << _JSONobj["action"].toString();
    }

}

void Client::onDocClosed()
{    
    m_document->deleteLater();

    connect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);
    QByteArray jsonString = ClientMessageFactory::createCloseEditorMessage();
    m_socket.sendTextMessage(jsonString);

    loginWindow.show();
}
