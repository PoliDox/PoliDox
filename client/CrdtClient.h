#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include "Char.h"
#include "CRDT.h"

class ClientController;

/* modella un'istanza di un editor condiviso */
class CrdtClient : public QObject,public CRDT {
    Q_OBJECT

private:    
    int _siteID;

    void printSymbols();

public:
    CrdtClient();

    void localInsert(unsigned int position, char value);
    void localDelete(unsigned int index);


    int getSiteId();

    void printDebugChars();

    static CrdtClient* fromJson(const QJsonObject& _JSONobj);

signals:
    void onLocalInsert(Char symbol);
    void onLocalDelete(Char symbol);
};


#endif // CLIENTCRDT_H
