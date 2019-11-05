#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <iostream>

#include <QSqlDatabase>
#include <QtSql>
#include <QCryptographicHash>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "Account.h"

#include "Char.h"

//TODO: - gestire eccezioni e controllare tipi di ritorno
//      - vedere mongodb autenticazione sicura  al db e chiavi primarie/autoincrement/foreign key/join
//      - fare un unico metodo generateBuilder in cui gli si passa un array di campi e uno di valori
//      - in realtà la macra Q_OBJECT non serve, da togliere!!



class DatabaseManager {

private:
    mongocxx::instance *instance;
    mongocxx::database *db;
    mongocxx::client *client;
    mongocxx::uri *uri;

    void incrementCounterOfCollection(QString nameCollection);
    int getCounterOfCollection(QString nameCollection);
    void insertNewElemInCounterCollection(QString nameDocument, int initialValue);

public:
    DatabaseManager();
    int registerUser(QString& name, QString& password, QByteArray& image);
    int checkPassword(QString& name, QString& password);
    bool insertNewDocument(QString& documentName, QString& uri);
    QString getUri(QString& documentName);
    QString getDocument(QString& uriOfDocument);
    bool insertNewPermission(QString& nameDocument, int siteId);
    bool insertSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition,
                      QString& family, int size, int bold, int italic, int underline, int alignment);
    bool deleteSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition);
    QList<Char> getAllInserts(QString& nameDocument);
    QList<QString> getAllDocuments(int siteId);
    QList<int> getAllAccounts(QString& nameDocument);
    QList<Account> getAllAccounts(QList<int>& siteIdAccounts);
    Account getAccount(int siteId);
    mongocxx::database getDb();
    ~DatabaseManager();
};



#endif // DATABASEMANAGER_H
