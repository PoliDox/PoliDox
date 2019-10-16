#include "ServerMessageFactory.h"
#include <QJsonObject>

QByteArray ServerMessageFactory::createNewClientMessage(const Account& account)
{
    QJsonObject l_obj;
    l_obj.insert("action", "newClient");    
    QJsonValue jsonAccount(account.toJson());
    l_obj.insert("account", jsonAccount);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}
