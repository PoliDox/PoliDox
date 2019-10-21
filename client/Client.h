#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "ClientController.h"
#include "Account.h"
#include "Log_dialog.h"
#include <QList>

class Client : public QObject
{
    Q_OBJECT

private:
    ClientController *m_document;
    QWebSocket m_socket;
    Account m_user;
    QList<QString> m_files;
    Log_Dialog loginWindow;

public:
    Client();

public slots:
    void onMessageReceived(const QString& p_msg);
};

#endif // CLIENT_H
