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

QByteArray ClientMessageFactory::createLoginMessage(const QString &p_user, const QString &p_passw)
{
    QJsonObject l_obj;
    l_obj.insert("action", "loginReq");
    l_obj.insert("username", p_user);
    l_obj.insert("password", p_passw);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createOpenFileMessage(const QString &p_filename)
{
    QJsonObject l_obj;
    l_obj.insert("action", "openFileReq");
    l_obj.insert("filename", p_filename);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}
