#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QJsonArray>
#include <QByteArray>
#include <QJsonObject>
#include <QColor>
#include <QPixmap>

// WARNING: This file is shared between client and server!
//          Please consider that before modifying

class Account {

private:
    int siteId;
    QString name;
    QByteArray image;

public:
    Account() = default;
    Account(int p_siteId, const QString& p_name, const QByteArray& p_picture);
    Account(const Account& other);
    int getSiteId() const;
    QString getName() const;
    QPixmap getImage() const;
    QJsonObject toJson() const;    
    static Account fromJson(const QJsonObject& accountJSON, bool isFromDb = false);
    bool operator < (const Account& other) const;
    bool operator == (const Account& other) const;


};

#endif // ACCOUNT_H
