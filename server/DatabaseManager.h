#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <iostream>
#include <sstream>

#include <QSqlDatabase>
#include <QtSql>
#include <QCryptographicHash>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include "Account.h"

#include "Char.h"


class DatabaseManager {

private:
    mongocxx::instance *instance;
    mongocxx::database *db;
    mongocxx::client *client;
    mongocxx::uri *uri;

    void incrementCounterOfCollection(QString nameCollection);
    int getCounterOfCollection(QString nameCollection);
    void insertNewElemInCounterCollection(QString nameDocument, int initialValue);
    QByteArray getImage(bsoncxx::document::view queryResult);

public:
    DatabaseManager();
    int registerUser(QString& name, QString& password, QByteArray& image);
    bool changeImage(QString& nameAccount, QByteArray& newImage);
    bool changePassword(QString& nameAccount, QString& password);
    int checkPassword(QString& name, QString& password, QByteArray& imageToReturn);
    bool insertNewDocument(QString& documentName, QString& uri);
    QString getUri(QString& documentName);
    QString getDocument(QString& uriOfDocument);
    bool insertNewPermission(QString& nameDocument, int siteId);
    void insertSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition,
                      QString& family, int size, bool bold, bool italic, bool underline, int alignment);
    void deleteSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition);
    QList<Char> getAllInserts(QString& nameDocument);
    QList<QString> getAllDocuments(int siteId);
    QList<int> getAllAccounts(QString& nameDocument);
    QList<Account> getAllAccounts(QList<int>& siteIdAccounts);
    Account getAccount(int siteId);
    mongocxx::database getDb();
    ~DatabaseManager();
};



#endif // DATABASEMANAGER_H
