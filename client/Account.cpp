#include "Account.h"



Account::Account(double siteId, const QString& name, const QByteArray& image)
    : siteId(siteId), name(name), image(image) {
    //nothing else to do (for now!!)
}


// - An account with siteId==-1 it's
//   a fake(or more precisely, a temporarily) account
Account::Account(const QString& name, const QByteArray& image)
    : Account(-1, name, image) {

}

double Account::getSiteId() const {
    return this->siteId;
}


QString Account::getName() const {
    return this->name;
}


QByteArray Account::getImage() const {
    return this->image;
}


QJsonObject Account::toJson() const {
    QJsonObject accountJSON;
    accountJSON.insert("siteId", siteId);
    accountJSON.insert("name", name);
    // TODO: how to send the picture? QByteArray cannot be transformed into a QJsonValue
    //_JSONobj.insert("image", image);

    return accountJSON;
}


Account Account::fromJson(const QJsonObject& accountJSON) {
    double siteId = accountJSON["siteId"].toDouble();
    QString name = accountJSON["name"].toString();
    return Account(siteId, name, "");
}

bool Account::operator < (const Account& other) const {
    return this->siteId < other.siteId;
}




