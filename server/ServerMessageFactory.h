#ifndef SERVERMESSAGEFACTORY_H
#define SERVERMESSAGEFACTORY_H

#include <QJsonDocument>


class ServerMessageFactory
{
public:
    ServerMessageFactory() = delete;

    static QByteArray createNewClientMessage(int p_siteId);

};


#endif // SERVERMESSAGEFACTORY_H
