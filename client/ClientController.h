#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include "CrdtClient.h"
#include "Editor.h"

class ClientController : public QObject
{
    Q_OBJECT

public:
    ClientController();
    ~ClientController();

private:
    Editor m_editor;
    CRDTclient *m_crdt; // TODO: use smart-pointer
    QWebSocket m_socket;

};

#endif // CLIENTCONTROLLER_H
