#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include <QObject>
#include "Char.h"
#include "CRDT.h"

class ClientController;

/* modella un'istanza di un editor condiviso */
class CrdtClient : public QObject, public CRDT {
    Q_OBJECT

private:    
    double siteId;
    void printSymbols();

public:
    CrdtClient(double siteId);
    void localInsert(unsigned int position, char value);
    void localDelete(unsigned int index);
    void printDebugChars();
    double getSiteId() const;
    void setSiteId(double siteId);

signals:
    void onLocalInsert(Char& symbol);
    void onLocalDelete(Char& symbol);
};


#endif // CLIENTCRDT_H
