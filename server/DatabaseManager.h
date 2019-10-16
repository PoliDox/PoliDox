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

//TODO: - gestire eccezioni e controllare tipi di ritorno
//      - vedere mongodb autenticazione sicura  al db e chiavi primarie/autoincrement/foreign key/join
//      - fare un unico metodo generateBuilder in cui gli si passa un array di campi e uno di valori



class DatabaseManager : public QObject {
    Q_OBJECT

private:
    mongocxx::instance *instance;
    mongocxx::database *db;
    mongocxx::client *client;
    mongocxx::uri *uri;

    //void updateCounterOfCollection(QString nameCollection);
    //int getCounterOfCollection(QString nameCollection);

public:
    void incrementCounterOfCollection(QString nameCollection);
    double getCounterOfCollection(QString nameCollection);

    DatabaseManager();
    bool registerUser(QString name, QString password, QByteArray image);
    bool checkPassword(QString name, QString password);
    bool insertNewDocument(QString documentName);
    bool insertSymbol(QString nameDocument, QString symbol, std::vector<int> fractionalPosition);
    bool deleteSymbol(QString nameDocument, QString symbol, std::vector<int> fractionalPosition);
    void retrieveAllInserts(QString nameDocument);
    mongocxx::database getDb();
    ~DatabaseManager();

};



#endif // DATABASEMANAGER_H
