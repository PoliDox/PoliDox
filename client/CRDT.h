#ifndef CRDT_H
#define CRDT_H

#include "Char.h"
#include <QObject>

class CRDT {

protected:
    std::vector<std::vector<Char>> _symbols;

    void _toMatrix(unsigned int position,unsigned int* row,unsigned int* index);
    int _toLinear(int row,int index);

    void mergeRows(std::vector<Char>& current,std::vector<Char>& next);
    void splitRows(std::vector<Char>& current,const unsigned int& row,const unsigned int& index);
    void insertSymbolAt(std::vector<Char>& row,Char& symbol,const unsigned int index);
    void deleteSymbolAt(std::vector<Char>& row,unsigned int index);
    void deleteRowAt(unsigned int row);
    void inserRowAtEnd(std::vector<Char>& row);
    void searchEqualSymbol(Char& symbol,unsigned int& _row,unsigned int& _index,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit);
    void searchGreaterSymbol(Char& symbol,unsigned int& _row,unsigned int& _index,int& _LINECOUNTER,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit);

public:
    CRDT();    
    int remoteInsert(Char& symbol);
    int remoteDelete(Char& symbol);
    QJsonArray toJson() const;
    static std::vector<std::vector<Char>> fromJson(const QJsonArray& crdtJsonFormatted);
    void fromJson(const QList<Char>& crdtJsonFormatted);
    void setSymbols(std::vector<std::vector<Char>> toSet);
    std::vector<std::vector<Char>> getSymbols();

};


#endif // CRDT_H
