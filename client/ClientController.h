#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include "CrdtClient.h"
#include "Editor.h"

class ClientController : public QObject {
    Q_OBJECT

public:
    ClientController(CrdtClient *p_crdt, QWebSocket *p_socket);
    ~ClientController();

private:
    Editor m_editor;
    CrdtClient *m_crdt; // TODO: use smart-pointer
    QWebSocket *m_socket;

public slots:
    void onTextMessageReceived(const QString& _JSONstring);

};

#endif // CLIENTCONTROLLER_H
