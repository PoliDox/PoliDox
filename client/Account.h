#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QByteArray>
#include <QJsonObject>

// WARNING: This file is shared between client and server!
//          Please consider that before modifying

class Account {
private:
    int m_siteId;
    QString m_name;
    QByteArray m_picture;

public:
    Account() = default;

    Account(int p_siteId, const QString& p_name, const QByteArray& p_picture)
        : m_siteId(p_siteId), m_name(p_name), m_picture(p_picture){ }

    int getSiteId() { return m_siteId; }
    QString getName() { return m_name; }
    QByteArray getPicture() { return m_picture; }

    QJsonObject toJson() const {
        QJsonObject _JSONobj;
        _JSONobj.insert("siteId", m_siteId);
        _JSONobj.insert("name", m_name);
        // TODO: how to send the picture? QByteArray cannot be transformed into a QJsonValue
        //_JSONobj.insert("picture", m_picture);

        return _JSONobj;
    }

    static Account fromJson(const QJsonObject& _JSONobj) {
        int siteId = _JSONobj["siteId"].toInt();
        QString name = _JSONobj["name"].toString();
        return Account(siteId, name, "");
    }

    bool operator <(const Account other) const {
        return m_siteId < other.m_siteId;
    }
};

#endif // ACCOUNT_H
