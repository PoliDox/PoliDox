#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QByteArray>
#include <QJsonObject>

// WARNING: This file is shared between client and server!
//          Please consider that before modifying

class Account {

private:
    double siteId;
    QString name;
    QByteArray image;

public:
    Account() = default;
    Account(double p_siteId, const QString& p_name, const QByteArray& p_picture);
    Account(const QString& p_name, const QByteArray& p_picture);
    double getSiteId() const;
    QString getName() const;
    QByteArray getImage() const;
    QJsonObject toJson() const;
    static Account fromJson(const QJsonObject& accountJSON);
    bool operator < (const Account& other) const;


};

#endif // ACCOUNT_H
