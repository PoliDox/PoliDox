#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include "CrdtClient.h"
#include "Editor.h"
#include "ListFiles.h"

class Editor;

class ClientController : public QObject {
    Q_OBJECT

public:
    ClientController(QWebSocket *p_socket, const Account& account, const QString& fileName, const QString& p_uri, QList<Account>& contributorsOnline, QList<Account>& contributorsOffline);
    ~ClientController();

    void init(const QJsonArray& p_crdt);
    QVector<int> getUserChars(int p_siteId);
    QString getFilename() { return m_filename; }
    QString getUri() { return m_uri; }
    Account getAccount() { return m_account; }

private:
    Editor *m_editor;
    CrdtClient *m_crdt;
    QWebSocket *m_socket;
    Account m_account;
    int m_siteId; // equivalent to m_account.getSiteId()
    QString m_filename;
    QString m_uri;
    QPixmap Pix;

public slots:
    void onTextMessageReceived(const QString& _JSONstring);
    void onTextChanged(int position, int charsRemoved, int charsAdded);

signals:        
    void docClosed();
    void docClosedNewFile();

};

#endif // CLIENTCONTROLLER_H
