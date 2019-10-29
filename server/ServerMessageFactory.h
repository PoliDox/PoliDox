#ifndef SERVERMESSAGEFACTORY_H
#define SERVERMESSAGEFACTORY_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include "Account.h"
#include "CRDT.h"

class ServerMessageFactory
{
public:
    ServerMessageFactory() = delete;

    static QByteArray createNewClientMessage(const Account *account);
    static QByteArray createLoginReply(bool response, const Account *account, QList<QString>& nameDocuments);
    static QByteArray createRegistrationUserReply(bool response, double siteIdOfRegisteredUser);
    static QByteArray createOpenFileReply(bool response, CRDT *crdt, QList<Account*>& accounts);
    static QByteArray createClosedEditorReply(QList<QString>& nameDocuments);
    static QByteArray createClosedEditorRemote(const Account *accountQuitted);
};


#endif // SERVERMESSAGEFACTORY_H
