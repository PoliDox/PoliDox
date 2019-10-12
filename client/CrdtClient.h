#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include "Char.h"

class ClientController;

/* modella un'istanza di un editor condiviso */
class CrdtClient : public QObject {
    Q_OBJECT

private:
    ClientController *m_controller;
    int _siteID;
    int _counter;
    std::vector<std::vector<Char>> _symbols;

    void _toMatrix(int position,int* row,int* index);
    int _toLinear(int row,int index);
    void printSymbols();

public:
    CrdtClient(ClientController *p_controller);

    void localInsert(int position, char value);
    void localDelete(int index);

    // These functions return the linear position
    // at which the character is added/deleted
    int remoteInsert(Char symbol);
    int remoteDelete(const Char& symbol);

    int getSiteId();
    int getCounter();
    int getCounterAndIncrement();

    void printDebugChars();

signals:
    void onLocalInsert(Char symbol);
    void onLocalDelete(Char symbol);    
};


#endif // CLIENTCRDT_H
