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
    ClientController(QWebSocket *p_socket, double p_siteId, QString fileName);
    ~ClientController();

    void init(const QJsonArray& p_crdt, const QJsonArray& p_accounts);
    QVector<int> getUserChars(int p_siteId);

private:
    Editor *m_editor;
    CrdtClient *m_crdt; // TODO: use smart-pointer
    QWebSocket *m_socket;
    ListFiles *m_lf;

public slots:
    void onTextMessageReceived(const QString& _JSONstring);
    void onTextChanged(int position, int charsRemoved, int charsAdded);

signals:
    void newUserOnline(Account account);

};

#endif // CLIENTCONTROLLER_H
