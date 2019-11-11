#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QByteArray>
#include <QJsonObject>
#include <QColor>
#include <QPixmap>

// WARNING: This file is shared between client and server!
//          Please consider that before modifying

class Account {
friend class DatabaseManager;
private:
    int siteId;
    QString name;
    QByteArray image;
    int color;   

public:
    Account() = default;
    Account(int p_siteId, const QString& p_name, const QByteArray& p_picture, int p_color = -1);
    Account(const Account& other);
    int getSiteId() const;
    QString getName() const;
    QPixmap getImage() const;
    QColor getColor() const;
    QJsonObject toJson() const;
    static Account fromJson(const QJsonObject& accountJSON);
    static int generateColor();
    bool operator < (const Account& other) const;
    bool operator == (const Account& other) const;


};

#endif // ACCOUNT_H
