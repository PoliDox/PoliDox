#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H

//
// Created by federico on 06/05/19.
//


#include <string>
#include <vector>
#include "Char.h"

/* modella un'istanza di un editor condiviso */
class CRDTclient {

private:
    int _siteID;
    int _counter;
    std::vector<std::vector<Char>> _symbols;

    void _toMatrix(int position,int* row,int* index);
    void printSymbols();

public:
    CRDTclient();


    void localInsert(int position, char value);
    void localDelete(int index);
    //void process(const Message& m);
    std::string toString();

    int getSiteId();
    int getCounter();
    int getCounterAndIncrement();

};


#endif // CLIENTCRDT_H
