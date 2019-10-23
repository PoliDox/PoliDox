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
    double getCounterOfCollection(QString nameCollection);
    void insertNewElemInCounterCollection(QString nameDocument, double initialValue);

public:
    DatabaseManager();
    double registerUser(QString& name, QString& password, QByteArray& image);
    double checkPassword(QString& name, QString& password);
    bool insertNewDocument(QString& documentName);
    bool insertSymbol(QString& nameDocument, QString& symbol, std::vector<int>& fractionalPosition);
    bool deleteSymbol(QString& nameDocument, QString& symbol, std::vector<int>& fractionalPosition);
    QList<Char> getAllInserts(QString& nameDocument);
    QList<QString> getAllDocuments();
    mongocxx::database getDb();
    ~DatabaseManager();
};



#endif // DATABASEMANAGER_H
