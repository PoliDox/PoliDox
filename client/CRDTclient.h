#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H

//
// Created by federico on 06/05/19.
//


#include <string>
#include <vector>
#include "symbol.h"
#include <QWebSocket>

/* modella un'istanza di un editor condiviso */
class CRDTclient {
private:
    int siteId;
    int counter;
    std::vector<Symbol> symbols;
    QWebSocket m_socket;

    void printSymbols();

public:
    CRDTclient();

    void localInsert(int index, char value);
    void localDelete(int index);
    //void process(const Message& m);
    std::string toString();

    int getSiteId();
    int getCounter();
    int getCounterAndIncrement();
    std::vector<Symbol> getSymbols();

};


#endif // CLIENTCRDT_H
