#include "Client.h"
#include "ClientMessageFactory.h"
#include "CrdtClient.h"
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include <QApplication>

Client::Client()
{
    m_socket.open(QUrl(QStringLiteral("ws://127.0.0.1:5678")));    

    connect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);

    connect(&loginWindow, &LoginWindow::authDataSubmitted, this, [&](QString p_user, QString p_passw) {
        QByteArray message = ClientMessageFactory::createLoginMessage(p_user, p_passw);
        m_socket.sendTextMessage(message);
    });

    connect(&loginWindow, &LoginWindow::signupDataSubmitted, this, [&](QString p_user, QString p_passw, QPixmap p_pic) {
        if (p_user != "You") {
            QByteArray message = ClientMessageFactory::createRegisterMessage(p_user, p_passw, p_pic);
            m_socket.sendTextMessage(message);
        } else {
            QMessageBox::warning(&loginWindow, "Registration", "\"You\" is not a valid username");
        }
    });

    connect(&loginWindow, &LoginWindow::fileSelected, this, [&](const QString& p_filename) {
        QByteArray message = ClientMessageFactory::createOpenFileMessage(p_filename);
        m_socket.sendTextMessage(message);
        // TODO: delete this, c_fileName should be filled at openFileRepl
        //c_fileName = p_filename;
    });

    connect(&loginWindow, &LoginWindow::uriSelected, this, [&](const QString& p_uri) {
        QByteArray message = ClientMessageFactory::createOpenFileMessage(QString(), p_uri);
        m_socket.sendTextMessage(message);
    });

    connect(&loginWindow, &LoginWindow::newFileSelected, this, [&](const QString& p_filename) {
        QByteArray message = ClientMessageFactory::createNewFileMessage(p_filename);
        m_socket.sendTextMessage(message);
    });

    QTimer::singleShot(4000, this, [&](){
        if (m_socket.state() != QAbstractSocket::ConnectedState) {
            QMessageBox::critical(&loginWindow, "Connection error", "Couldn't connect to server. Please close the application and retry.");
            QApplication::quit();
        }

    });

    loginWindow.show();

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
            QMessageBox::warning(&loginWindow, "Login", "Username or password are incorrect");
            return;
        }

        QJsonObject accountObj = _JSONobj["account"].toObject();
        m_user = Account::fromJson(accountObj);
        qDebug() << "Authenticated with siteId" << m_user.getSiteId();
        int r, g, b;
        m_user.getColor().getRgb(&r, &g, &b);        

        QJsonArray _JSONfiles=_JSONobj["nameDocuments"].toArray();
        QList<QString> l_files;
        for(QJsonArray::iterator it=_JSONfiles.begin();it!=_JSONfiles.end();it++){
            l_files.push_back(it->toString());
        }
        loginWindow.displayFiles(l_files);

    } else if (l_header == "registerUserRepl") {
        QString replCode = _JSONobj["response"].toString();
        if (replCode != "ok") {
            QMessageBox::warning(&loginWindow, "Registration", "Couldn't create user: username already exists");
            return;
        }
        loginWindow.registrationOk();

    } else if (l_header == "openFileRepl") {
        QString replCode = _JSONobj["response"].toString();

        if (replCode == "ok") {
            //qDebug() << "JSON ARRIVATO:" << p_msg.toUtf8().constData();
            QString nameDocument = _JSONobj["nameDocument"].toString();
            QString uri = _JSONobj["uri"].toString();
            QJsonArray JSONcrdt = _JSONobj["crdt"].toArray();

            QJsonArray JSONaccountsOnline = _JSONobj["accounts"].toArray();
            QJsonArray JSONaccountsOffline = _JSONobj["accountsOffline"].toArray();
            QList<Account> contributorsOnline;
            QList<Account> contributorsOffline;           
            for (const QJsonValue accOnline : JSONaccountsOnline)
                contributorsOnline.push_back(Account::fromJson(accOnline.toObject()));
            for (const QJsonValue accOffline : JSONaccountsOffline)
                contributorsOffline.push_back(Account::fromJson(accOffline.toObject()));

            m_document = new ClientController(&m_socket, m_user, nameDocument, uri, contributorsOnline, contributorsOffline);
            m_document->init(JSONcrdt);

            loginWindow.hide();
            disconnect(&m_socket, &QWebSocket::textMessageReceived, this, &Client::onMessageReceived);

            connect(m_document, &ClientController::docClosed, this, &Client::onDocClosed);

        } else if (replCode == "fail create") {
            // TODO: chiedere a fede: i filenames sono unici a livello di sistema o a livello account?
            QMessageBox::warning(&loginWindow, "New file", "Couldn't create file: filename is already used");
        } else if (replCode == "fail uri") {
            QMessageBox::warning(&loginWindow, "Open file", "Couldn't open file: Uri is invalid");
        } else if (replCode == "fail name") {
            QMessageBox::warning(&loginWindow, "Open file", "Couldn't open file: File name is invalid");
        }

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
