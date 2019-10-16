#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include "Char.h"
#include "CRDT.h"

class ClientController;

/* modella un'istanza di un editor condiviso */
class CrdtClient : public CRDT {
    Q_OBJECT

private:
    ClientController *m_controller;
    int _siteID;

    void printSymbols();

public:
    CrdtClient(ClientController *p_controller);

    void localInsert(unsigned int position, char value);
    void localDelete(unsigned int index);


    int getSiteId();

    void printDebugChars();

signals:
    void onLocalInsert(Char symbol);
    void onLocalDelete(Char symbol);    
};


#endif // CLIENTCRDT_H
