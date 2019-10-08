#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H

#include <QObject>
#include <QWebSocket>
#include <QJsonObject>
#include "CRDTclient.h"
#include "editor.h"

/* ========================================================================================
   This class has:

               -pointer to CRDTclient: one CLIENTcontroller is associated
                                       with one CRDTclient and it has to prepare
                                       the message that will be sent to the sever
                                       through the socket.

               - socket: used to communicate with the server.

   ========================================================================================  */

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
