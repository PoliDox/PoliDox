#include "Account.h"
#include <QDebug>
#include <cstdlib>

Account::Account(int p_siteId, const QString& p_name, const QByteArray& p_image, int p_color)
    : siteId(p_siteId), name(p_name), image(p_image) {

    if (p_color < 0) { // default value
        color = generateColor();
    } else {
        color = p_color;
    }
}


Account::Account(const Account& other){
    this->siteId = other.siteId;
    this->name = other.name;
    this->image = other.image;
    this->color = other.color;
}


int Account::getSiteId() const {
    return this->siteId;
}


QString Account::getName() const {
    return this->name;
}

QPixmap Account::getImage() const {
    QPixmap p;
    p.loadFromData(QByteArray::fromBase64(image), "PNG");
    return p;
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
    accountJSON.insert("image", QLatin1String(image));
    return accountJSON;
}


Account Account::fromJson(const QJsonObject& accountJSON) {
    int l_siteId = accountJSON["siteId"].toInt();
    QString l_name = accountJSON["name"].toString();
    if(l_name == "")
        l_name = accountJSON["_id"].toString(); //non cancellare, serve per la conversione dal db  =)
    int l_color = accountJSON["color"].toInt();
    QByteArray l_image = accountJSON["image"].toString().toLatin1();
    qDebug() << "ACCOUNT " << accountJSON ;
    return Account(l_siteId, l_name, l_image, l_color);
}

int Account::generateColor()
{
    int ret;
    for (int i=0; i < 3; i++) {
        int randomNr = rand() % 200; // We don't want to get any color near to white
        randomNr = randomNr << i*8; // We save the color as a single hex value
        ret |= randomNr;
    }
    qDebug() << "Generated color: " << ret;
    return ret;
}

bool Account::operator < (const Account& other) const {
    return this->siteId < other.siteId;
}


bool Account::operator == (const Account& other) const {
    return this->siteId == other.siteId;
}


