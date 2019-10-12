#include "ServerMessageFactory.h"
#include <QJsonObject>

QByteArray ServerMessageFactory::createNewClientMessage(int p_siteId)
{
    QJsonObject l_obj;
    l_obj.insert("action", "newClient");
    l_obj.insert("siteId", p_siteId);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}
