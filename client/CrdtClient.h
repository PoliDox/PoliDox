#ifndef CLIENTCRDT_H
#define CLIENTCRDT_H


#include <string>
#include <vector>
#include <QWebSocket>
#include "Char.h"

class ClientController;

/* modella un'istanza di un editor condiviso */
class CRDTclient : public QObject {
    Q_OBJECT

private:
    ClientController *m_controller; //TODO use smart pointer
    int _siteID;
    int _counter;
    std::vector<std::vector<Char>> _symbols;    

    void _toMatrix(int position,int* row,int* index);
    void printSymbols();

public:
    CRDTclient(ClientController *p_controller);


    void localInsert(int position, char value);
    void remoteDelete(const Char& symbol);
    
    void remoteInsert(Char symbol);
    //void process(const Message& m);
    std::string toString();

    int getSiteId();
    int getCounter();
    int getCounterAndIncrement();

signals:
    void onLocalInsert(Char symbol); // TODO: Add parameters

};


#endif // CLIENTCRDT_H
