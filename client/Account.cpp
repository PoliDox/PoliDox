#include "Account.h"
#include <QDebug>
#include <cstdlib>
#include <QBuffer>

Account::Account(int p_siteId, const QString& p_name, const QByteArray& p_image)
    : siteId(p_siteId), name(p_name), image(p_image)
{}


Account::Account(const Account& other){
    this->siteId = other.siteId;
    this->name = other.name;
    this->image = other.image;    
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

QJsonObject Account::toJson() const {
    QJsonObject accountJSON;
    accountJSON.insert("siteId", siteId);
    accountJSON.insert("name", name);
    accountJSON.insert("image", QLatin1String(image));
    return accountJSON;
}

Account Account::fromJson(const QJsonObject& accountJSON, bool isFromDb) {
    int l_siteId = accountJSON["siteId"].toInt();
    QString l_name = accountJSON["name"].toString();
    if(l_name == "")
        l_name = accountJSON["_id"].toString(); // do not delete: it is used for the conversion from db

    QByteArray l_image;
    if(!isFromDb){
        l_image = accountJSON["image"].toString().toLatin1();
    }
    else {
        QJsonArray imageObjJSON = accountJSON["image"].toArray();
        for(auto elem : imageObjJSON){
            char appo = (char)elem.toInt();
            l_image.push_back(appo);
        }
    }

    return Account(l_siteId, l_name, l_image);
}


bool Account::operator < (const Account& other) const {
    return this->siteId < other.siteId;
}

bool Account::operator == (const Account& other) const {
    return this->siteId == other.siteId;
}

void Account::setImage(QPixmap Pix){
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    Pix.save(&buffer, "PNG");
    auto const encoded = buffer.data().toBase64();
    this->image = QLatin1String(encoded).latin1();
}
