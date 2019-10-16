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
};

#endif // MESSAGEFACTORY_H
