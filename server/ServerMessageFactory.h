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
    static QByteArray createLoginReply(bool response, QString typeError, const Account *account, QList<QString>& nameDocuments);
    static QByteArray createRegistrationUserReply(bool response, double siteIdOfRegisteredUser);    
    static QByteArray createChangeImageReply(bool response);
    static QByteArray createChangePasswordReply(bool response);
    static QByteArray createOpenFileReply(bool response, QString typeError, QString filename, QString uri, CRDT *crdt, QList<Account*>& accounts, QList<Account>& accountsOffline);
    static QByteArray createOpenFileReply(bool response, QString typeError);
    static QByteArray createClosedEditorReply(QList<QString>& nameDocuments);
    static QByteArray createClosedEditorRemote(const Account *accountQuitted);
    static QByteArray createGetUriReply(QString& nameDocument, QString& uri);
    static QByteArray createPermissionReply(bool response, QString& nameDocument);
};


#endif // SERVERMESSAGEFACTORY_H
