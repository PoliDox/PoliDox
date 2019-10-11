#ifndef MESSAGEFACTORY_H
#define MESSAGEFACTORY_H

#include <QJsonDocument>
#include "Char.h"

class MessageFactory
{
public:    
    MessageFactory() = delete;

    static QByteArray createInsertMessage(const Char& p_char);
    static QByteArray createDeleteMessage(const Char& p_char);
};

#endif // MESSAGEFACTORY_H
