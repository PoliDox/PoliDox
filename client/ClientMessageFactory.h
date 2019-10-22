#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

#include <QJsonDocument>
#include "Char.h"

class ClientMessageFactory
{
public:    
    ClientMessageFactory() = delete;

    static QByteArray createInsertMessage(const Char& p_char);
    static QByteArray createDeleteMessage(const Char& p_char);
    static QByteArray createRegisterMessage(const QString& p_user, const QString& p_passw);
    static QByteArray createLoginMessage(const QString& p_user, const QString& p_passw);
    static QByteArray createOpenFileMessage(const QString& p_filename);
    static QByteArray createNewFileMessage(const QString& p_filename);

};

#endif // MESSAGEFACTORY_H
