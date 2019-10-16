#ifndef SERVERMESSAGEFACTORY_H
#define SERVERMESSAGEFACTORY_H

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QString>
#include "Account.h"

class ServerMessageFactory
{
public:
    ServerMessageFactory() = delete;

    static QByteArray createNewClientMessage(const Account& account);
    static QByteArray createLoginReply(bool response, const Account *account, QList<QString>& nameDocuments);
    static QByteArray createRegistrationUserReply(double siteIdOfRegisteredUser);

};


#endif // SERVERMESSAGEFACTORY_H
