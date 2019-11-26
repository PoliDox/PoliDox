#include "ClientMessageFactory.h"
#include <QBuffer>
#include "qdebug.h"

QByteArray ClientMessageFactory::createInsertMessage(const Char& p_char, int p_siteId)
{
    QJsonObject l_obj;    
    l_obj.insert("action", "insert");

    QJsonValue jsonChar(p_char.toJson());
    l_obj.insert("char", jsonChar);

    l_obj.insert("siteId", p_siteId);

    QJsonDocument l_doc(l_obj);        

    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createDeleteMessage(const Char &p_char, int p_siteId)
{
    QJsonObject l_obj;    
    l_obj.insert("action", "delete");

    QJsonValue jsonChar(p_char.toJson());
    l_obj.insert("char", jsonChar);

    l_obj.insert("siteId", p_siteId);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createCursorMoveMessage(int p_pos, int p_siteId)
{
    QJsonObject l_obj;
    l_obj.insert("action", "cursorMove");
    l_obj.insert("siteId", p_siteId);
    l_obj.insert("position", p_pos);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createRegisterMessage(const QString &p_user, const QString &p_passw, const QPixmap& p_pic)
{
    QJsonObject l_obj;
    l_obj.insert("action", "registerUser");
    l_obj.insert("name", p_user);
    l_obj.insert("password", p_passw);
    // insert image
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    p_pic.save(&buffer, "PNG");
    auto const encoded = buffer.data().toBase64();
    l_obj.insert("image", QLatin1String(encoded));

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createLoginMessage(const QString &p_user, const QString &p_passw)
{
    QJsonObject l_obj;
    l_obj.insert("action", "loginReq");
    l_obj.insert("name", p_user);
    l_obj.insert("password", p_passw);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createOpenFileMessage(const QString &p_filename, const QString &p_uri)
{
    QJsonObject l_obj;
    l_obj.insert("action", "openFileReq");
    l_obj.insert("nameDocument", p_filename);
    l_obj.insert("uri", p_uri);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createNewFileMessage(const QString &p_filename)
{
    QJsonObject l_obj;
    l_obj.insert("action", "createFileReq");
    l_obj.insert("nameDocument", p_filename);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createCloseEditorMessage(){
    QJsonObject l_obj;
    l_obj.insert("action", "closedEditorReq");

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}

QByteArray ClientMessageFactory::createImgUpdate(const QString &p_user, const QPixmap &p_pic){
    QJsonObject l_obj;
    l_obj.insert("action", "changeImgReq");
    l_obj.insert("nameAccount", p_user);
    // insert image
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    p_pic.save(&buffer, "PNG");
    auto const encoded = buffer.data().toBase64();
    l_obj.insert("newImg", QLatin1String(encoded));

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}
