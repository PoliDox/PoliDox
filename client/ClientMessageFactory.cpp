#include "ClientMessageFactory.h"


QByteArray ClientMessageFactory::createInsertMessage(const Char& p_char)
{
    QJsonObject l_obj;    
    l_obj.insert("action", "insert");

    QJsonValue jsonChar(p_char.toJson());
    l_obj.insert("char", jsonChar);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createDeleteMessage(const Char &p_char)
{
    QJsonObject l_obj;    
    l_obj.insert("action", "delete");

    QJsonValue jsonChar(p_char.toJson());
    l_obj.insert("char", jsonChar);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}
