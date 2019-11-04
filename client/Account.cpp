#include "Account.h"
#include <QDebug>
#include <cstdlib>

Account::Account(int p_siteId, const QString& p_name, const QByteArray& p_image, int p_color)
    : siteId(p_siteId), name(p_name), image(p_image) {

    if (p_color < 0) { // default value
        color = 0;
        for (int i=0; i < 3; i++) {
            int randomNr = rand() % 200; // We don't want to get any color near to white
            randomNr = randomNr << i*8; // We save the color as a single hex value
            color |= randomNr;            
        }
    } else {
        color = p_color;
    }
}

int Account::getSiteId() const {
    return this->siteId;
}


QString Account::getName() const {
    return this->name;
}

QByteArray Account::getImage() const {
    return this->image;
}

QColor Account::getColor() const
{    
    int red, green, blue;
    blue = color & 0xFF;
    green = (color >> 8) & 0xFF;
    red = (color >> 16) & 0xFF;
    return QColor(red, green, blue);
}

QJsonObject Account::toJson() const {
    QJsonObject accountJSON;
    accountJSON.insert("siteId", siteId);
    accountJSON.insert("name", name);
    accountJSON.insert("color", color);
    //qDebug() << "JsonColorServer: " << hex << color;
    // TODO: how to send the picture? QByteArray cannot be transformed into a QJsonValue
    //_JSONobj.insert("image", image);

    return accountJSON;
}


Account Account::fromJson(const QJsonObject& accountJSON) {
    int l_siteId = accountJSON["siteId"].toInt();
    QString l_name = accountJSON["name"].toString();
    int l_color = accountJSON["color"].toInt();
    //qDebug() << "JsonColorClient: " << hex << l_color;
    qDebug() << "ACCOUNT " << accountJSON ;
    return Account(l_siteId, l_name, "", l_color);
}

bool Account::operator < (const Account& other) const {
    return this->siteId < other.siteId;
}




