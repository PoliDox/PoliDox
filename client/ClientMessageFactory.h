#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

#include <QJsonDocument>
#include <QPixmap>
#include "Char.h"

class ClientMessageFactory
{
public:    
    ClientMessageFactory() = delete;

    static QByteArray createInsertMessage(const Char& p_char, int p_siteId);
    static QByteArray createDeleteMessage(const Char& p_char, int p_siteId);
    static QByteArray createRegisterMessage(const QString& p_user, const QString& p_passw, const QPixmap& p_pic);
    static QByteArray createLoginMessage(const QString& p_user, const QString& p_passw);
    static QByteArray createOpenFileMessage(const QString& p_filename, const QString& p_uri = QString());
    static QByteArray createNewFileMessage(const QString& p_filename);
    static QByteArray createCloseEditorMessage();

};

#endif // MESSAGEFACTORY_H
