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
    void printSymbols();

public:
    CrdtClient(ClientController *p_controller);

    void printDebugChars();
    void localInsert(int position, char value);
    void remoteInsert(Char symbol);
    void localDelete(int index);
    //void process(const Message& m);

    void remoteDelete(const Char& symbol);
    std::string toString();

    int getSiteId();
    int getCounter();
    int getCounterAndIncrement();

signals:
    void onLocalInsert(Char symbol); // TODO: Add parameters
    void onLocalDelete(Char symbol);

};


#endif // CLIENTCRDT_H
