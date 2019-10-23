#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include "CrdtClient.h"
#include "Editor.h"
#include "ListFiles.h"

class ClientController : public QObject {
    Q_OBJECT

public:
    ClientController(CrdtClient *p_crdt, QWebSocket *p_socket, const QList<Account>& accounts);
    ~ClientController();

private:
    Editor m_editor;
    CrdtClient *m_crdt; // TODO: use smart-pointer
    QWebSocket *m_socket;

public slots:
    void onTextMessageReceived(const QString& _JSONstring);
    void onTextChanged(int position, int charsRemoved, int charsAdded);

};

#endif // CLIENTCONTROLLER_H
