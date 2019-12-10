#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include <QObject>
#include "Char.h"
#include "CRDT.h"

class ClientController;

class CrdtClient : public QObject, public CRDT{
    Q_OBJECT

private:    
    int siteId;
    void printSymbols();

public:
    CrdtClient(int p_siteId);
    void localInsert(unsigned int position, Char symbolToInsert);
    void localDelete(unsigned int index);
    int getSiteId() const;
    void setSiteId(int siteId);
    std::vector<int> getUserPositions(int siteId);

signals:
    void onLocalInsert(Char& symbol);
    void onLocalDelete(Char& symbol);
};

#endif // CLIENTCRDT_H
