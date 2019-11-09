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
    ClientController(QWebSocket *p_socket, double p_siteId, const QString& fileName, const QString& p_uri, QList<Account>& contributorsOnline, QList<Account>& contributorsOffline);
    ~ClientController();

    void init(const QJsonArray& p_crdt);
    QVector<int> getUserChars(int p_siteId);
    QString getFilename() { return m_filename; }
    QString getUri() { return m_uri; }


private:
    Editor *m_editor;
    CrdtClient *m_crdt; // TODO: use smart-pointer
    QWebSocket *m_socket;
    int m_siteId;
    QString m_filename;
    QString m_uri;

public slots:
    void onTextMessageReceived(const QString& _JSONstring);
    void onTextChanged(int position, int charsRemoved, int charsAdded);

signals:
    void newUserOnline(Account account);
    void userOffline(Account account);
    void docClosed();

};

#endif // CLIENTCONTROLLER_H
