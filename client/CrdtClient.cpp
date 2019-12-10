    #include "CrdtClient.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QObject>
#include <future>
#include <QDate>
#include <QDateTime>



#define MAXNUM INT_MAX
#define DIVISORE 2
//#define DEBUG_OUTPUT

CrdtClient::CrdtClient(int p_siteId) : siteId(p_siteId)
{}

int CrdtClient::getSiteId() const {
    return this->siteId;
}


void CrdtClient::setSiteId(int siteID) {
    this->siteId = siteID;
}

std::vector<int> CrdtClient::getUserPositions(int siteId)
{
    unsigned int i=0,
                 j=0;

    std::vector<int> result;
    for(i=0;i<_symbols.size();i++){
        for(j=0;j<_symbols[i].size();j++){
            if(_symbols[i][j].getSiteId()==siteId){
                    result.push_back(_toLinear(i,j));
            }
        }
    }

    return result;
}

bool existsPositionInVector(unsigned int position, std::vector<int> vector) {

    return position < vector.size();
}

std::vector<int> createMiddleFractionalNumber(std::vector<int> preceding, std::vector<int> following){

    std::vector<int> middle;

     unsigned int i=0, j=0;


    unsigned long moreSmallVector, precedingSize=preceding.size(), followingSize=following.size();
    if(precedingSize < followingSize)
        moreSmallVector = precedingSize;
    else
        moreSmallVector = followingSize;


    for(i=0; i<moreSmallVector; i++){

        int difference = following[i] - preceding[i];
        if(difference > 1) {
            int middleElement = (difference) / DIVISORE;
            middleElement = middleElement + preceding[i];
            middle.push_back(middleElement);
            return middle;
        }
        else {
            middle.push_back(preceding[i]);
            if(difference == 1){
                if( existsPositionInVector(i+1, preceding) ) {  //devo trovare elemento più grande di precedeing
                    for(j=i+1; j<precedingSize; j++) {
                        if (preceding[j] <= (MAXNUM - 2)) {
                            int middleElement = (MAXNUM - preceding[j]) / DIVISORE;
                            middleElement = middleElement + preceding[j];
                            middle.push_back(middleElement);
                            return middle;
                        } else {
                            middle.push_back(preceding[j]);     //copia preceding finchè non trovi elemento più grande da inserire
                        }
                    }
                    middle.push_back(MAXNUM/2);
                    return middle;
                }
                else {
                    middle.push_back(MAXNUM/2);
                    return middle;
                }
            }
        }

    }


    //preceding è finito ma following ancora no
    bool precedingIsFinish = i==precedingSize;

    if(precedingIsFinish){
        for(; i<followingSize; i++) {
            if (following[i] == 0) {
                middle.push_back(following[i]);
            }
            else {
                if(following[i] == 1){
                    middle.push_back(0);
                    middle.push_back(MAXNUM/2);
                    return middle;
                }
                else {
                    int middleElement = following[i] / DIVISORE;
                    middle.push_back(middleElement);
                    return middle;
                }
            }
        }
    }

    //NON DOVREI MAI ARRIVARE QUI
    qCritical() << "ERRORE\n"; //NON DOVREI MIA ENTRARE QUI
    throw std::exception();

}


void CrdtClient::localInsert(unsigned int position, Char symbolToInsert) {


    unsigned int row=0,
                 index=0,
                 middleNewLine=0;

    unsigned long rowSize=0;

    ushort value;

    value=symbolToInsert.getValue();

    this->_toMatrix(position,&row,&index);

    // Se la row è maggiore della dimensione della matrice
    // allora stiamo inserendo in una nuova riga.


    if(row>=this->_symbols.size()){
        this->_symbols.insert(this->_symbols.begin() + (row), std::vector<Char>());
    }

#ifdef DEBUG_OUTPUT
    if(value=='\n')
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t\\n "<<"\tLINEAR POSITION " << position<< std::endl;
    else
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t"<< value <<"\tLINEAR POSITION " << position << std::endl;
#endif

    int siteID=symbolToInsert.getSiteId();
    int random=rand();

    rowSize=this->_symbols[row].size();

    if(value=='\n'){
        if (index != this->_symbols[row].size())        //newline in mezzo ad una riga
            middleNewLine=1;
    }

    if(index == 0){                                     //primo elemento della riga
        if(rowSize == 0){                               //primo della riga e nessuno dopo di me
            std::vector<int> precedingFractionalNumber;
            std::vector<int> firstElem{MAXNUM/2};
            std::vector<int> fakeVector{MAXNUM};

            if(row!=0){                                 //primo della riga ma non riga 0 e nessuno dopo di me
                precedingFractionalNumber = this->_symbols[row - 1].at(this->_symbols[row - 1].size() - 1).getFractionalPosition();
                std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber,fakeVector);
                newFractionalPosition.push_back(siteID);
                newFractionalPosition.push_back(random);
                symbolToInsert.setFractionalPosition(newFractionalPosition);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);

            }else{                                      //primo della riga e siamo su riga 0 e nessuno dopo di me
                firstElem.push_back(siteID);
                firstElem.push_back(random);
                symbolToInsert.setFractionalPosition(firstElem);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
            }
        }
        else {                                          //primo della riga ma ho qualcuno dopo di me
            std::vector<int> precedingFractionalNumber;
            if(row!=0)                                  //primo della riga, qualcuno dopo di me, riga !=0
                precedingFractionalNumber = this->_symbols[row-1].at(this->_symbols[row-1].size()-1).getFractionalPosition();
            else                                        //primo della riga, qualcuno dopo di me, riga 0
                 precedingFractionalNumber.push_back(0);

            std::vector<int> followingFractionalNumber = this->_symbols[row].at(0).getFractionalPosition();
            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            newFractionalPosition.push_back(siteID);
            newFractionalPosition.push_back(random);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
    }
    else {                                              //non sono il primo della riga
        if(index >= rowSize){                           //non ho nessuno dopo di me
            index = static_cast<unsigned int>(rowSize);
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> fakeVector{MAXNUM};

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, fakeVector);
            newFractionalPosition.push_back(siteID);
            newFractionalPosition.push_back(random);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
        else {                                          //sono in mezzo a due simboli
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> followingFractionalNumber = this->_symbols[row].at(index).getFractionalPosition();

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            newFractionalPosition.push_back(siteID);
            newFractionalPosition.push_back(random);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
    }

    if(middleNewLine==1) {
        this->_symbols.insert(this->_symbols.begin() + (row + 1),
                               std::vector<Char>(this->_symbols[row].begin() + index + 1, this->_symbols[row].end()));
        this->_symbols[row].erase(this->_symbols[row].begin() + index + 1, this->_symbols[row].end());

    }

    emit onLocalInsert(symbolToInsert);

}



void CrdtClient::localDelete(unsigned int position){

    unsigned int row=0,
                 index=0,
                 _NROWS=static_cast<unsigned int>(this->_symbols.size());

    this->_toMatrix(position,&row,&index);

    if(row>_NROWS-1)    //paragraph alignment
        return;

    Char _Dsymbol=this->_symbols[row][index];
    ushort _CHAR=_Dsymbol.getValue();

    if(_CHAR=='\n' &&_NROWS!=1 && row!=_NROWS-1){
        mergeRows(this->_symbols[row],this->_symbols[row+1]);
        deleteRowAt(row+1);
    }else
        deleteSymbolAt(this->_symbols[row],index);

    if(this->_symbols[row].size()==0 && this->_symbols.size()>1) //editor empty=one empty row so don't clear last row
        deleteRowAt(row);


    emit onLocalDelete(_Dsymbol);

#ifdef DEBUG_OUTPUT
     if(_CHAR=='\n')
         std::cout << "[LOCAL DELETE]@ [" << row << "] [" << index <<"]\t\\n"<< std::endl;
     else
         std::cout << "[LOCAL DELETE]@ [" << row << "] [" << index <<"]\t" << _Dsymbol.getValue() << std::endl;
#endif


}


