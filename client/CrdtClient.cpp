#include "CrdtClient.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QObject>
#include <future>
#include <QDate>
#include <QDateTime>



#define MAXNUM 100
#define DEBUG_OUTPUT


double CrdtClient::getSiteId() const {
    return this->siteId;
}


void CrdtClient::setSiteId(double siteID) {
    this->siteId = siteID;
}


CrdtClient::CrdtClient(double siteId) {
    //TODO il vettore di simboli inizialmente è vuoto??
    this->siteId = siteId;
}

bool existsPositionInVector(unsigned int position, std::vector<int> vector) {
    if(position < vector.size())
        return true;

        return false;
}

/* assumo che sia preceding che following
 * abbiamo almeno un elemento    */
std::vector<int> createMiddleFractionalNumber(std::vector<int> preceding, std::vector<int> following){

    std::vector<int> middle;

     unsigned int i=0,
                  j=0;


    unsigned long moreSmallVector, precedingSize=preceding.size(), followingSize=following.size();
    if(precedingSize < followingSize)
        moreSmallVector = precedingSize;
    else
        moreSmallVector = followingSize;


    for(i=0; i<moreSmallVector; i++){

        int difference = following[i] - preceding[i];
        if(difference > 1) {
            int middleElement = (difference) / 2;
            middleElement = middleElement + preceding[i];
            middle.push_back(middleElement);
            return middle;
        }
        else {
            middle.push_back(preceding[i]);
            if(difference == 1){
                //qui terminerÃ² sicuro, devo solo trovare un numero piÃ¹
                //grande di preceding da poter inserire
                if( existsPositionInVector(i+1, preceding) ) {
                    for(j=i+1; j<precedingSize; j++) {
                        if (preceding[j] <= (MAXNUM - 2)) {
                            int middleElement = (MAXNUM - preceding[j]) / 2;
                            middleElement = middleElement + preceding[j];
                            middle.push_back(middleElement);
                            return middle;
                        } else {
                            //copia preceding finchÃ© non trovi
                            //un elemento i-esimo piÃ¹ grande da poter inserire.
                            middle.push_back(preceding[j]);
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

    //se arrivo qui Ã¨ perchÃ© sicuramente
    //preceding Ã¨ finito e following ancora no!!
    //TODO da confermare se Ã¨ vero

    bool precedingIsFinish = i==precedingSize;
    bool followingIsFinish = i==followingSize;

    if( precedingIsFinish && followingIsFinish){
        std::cout << "ERRORE\n"; //NON DOVREI MIA ENTRARE QUI
        exit(10);  //10: numero a caso
    }

    if(precedingIsFinish){
        //preceding Ã¨ finito e following no
        for(; i<followingSize; i++) {
            if (following[i] == 0) {  //puÃ² essere sia 1 che zero
                middle.push_back(following[i]);
            }
            else {
                if(following[i] == 1){
                    middle.push_back(0);
                    middle.push_back(MAXNUM/2);
                    return middle;
                }
                else {
                    int middleElement = following[i] / 2;
                    middle.push_back(middleElement);
                    return middle;
                }
            }
        }
    }

    //NON DOVREI MAI ARRIVARE QUI
    std::cout << "ERRORE\n"; //NON DOVREI MIA ENTRARE QUI
    exit(10);  //10: numero a caso

}


/* position Ã¨ il valore restituito dall'editor QT, va convertito in row e index della matrice CRDT */

void CrdtClient::localInsert(unsigned int position, char value) {

    unsigned int row=0,
                 index=0,
                 middleNewLine=0;

    unsigned long rowSize=0;

    //QDateTime asd(QDateTime::currentDateTime());
    //uint unixTime = asd.toTime_t();

    this->_toMatrix(position,&row,&index);

    /* !!! SOLUTION TO 2ND BUG ON LOCAL INSERT !!! */
    if(row>=this->_symbols.size()){
        this->_symbols.insert(this->_symbols.begin() + (row), std::vector<Char>());
    }

#ifdef DEBUG_OUTPUT
    if(value=='\n')
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t\\n "<<"\tLINEAR POSITION " << position<< std::endl;
    else
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t"<< value <<"\tLINEAR POSITION " << position << std::endl;
#endif

    Char symbolToInsert(this->siteId, 0, value);

    rowSize=this->_symbols[row].size();

    if(value=='\n'){
        if (index != this->_symbols[row].size())
            middleNewLine=1;
    }

    if(index == 0){
        if(rowSize == 0){
            std::vector<int> precedingFractionalNumber;
            //è il primo elemento che inserisco
            std::vector<int> firstElem{MAXNUM/2};
            std::vector<int> fakeVector{MAXNUM};

            if(row!=0){
                precedingFractionalNumber = this->_symbols[row - 1].at(this->_symbols[row - 1].size() - 1).getFractionalPosition();
                std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber,fakeVector);
                //newFractionalPosition.push_back(unixTime);
                symbolToInsert.setFractionalPosition(newFractionalPosition);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);

            }else{
                //firstElem.push_back(unixTime);
                symbolToInsert.setFractionalPosition(firstElem);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
            }
        }
        else {
            //ho degli elementi dopo di me
            std::vector<int> precedingFractionalNumber;
            if(row!=0)
                precedingFractionalNumber = this->_symbols[row-1].at(this->_symbols[row-1].size()-1).getFractionalPosition();
            else
                 precedingFractionalNumber.push_back(0); //sarebbe il fake vector

            std::vector<int> followingFractionalNumber = this->_symbols[row].at(0).getFractionalPosition();
            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            //newFractionalPosition.push_back(unixTime);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
    }
    else {
        //sicuramente avrà un simbolo prima di me
        if(index >= rowSize){  //TODO controllare bene il confronto
            //non ho nessuno dopo di me
            index = rowSize;  //inserisco in append
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> fakeVector{MAXNUM};

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, fakeVector);
            //newFractionalPosition.push_back(unixTime);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
        else {
            //sono fra due simboli
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> followingFractionalNumber = this->_symbols[row].at(index).getFractionalPosition();

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            //newFractionalPosition.push_back(unixTime);
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
                 _NROWS=this->_symbols.size();

    this->_toMatrix(position,&row,&index);

    if(row>_NROWS-1)    //paragraph alignment
        return;

    Char _Dsymbol=this->_symbols[row][index];
    char _CHAR=_Dsymbol.getValue();

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


void CrdtClient::printDebugChars(){
    std::for_each(_symbols.begin(), _symbols.end(), [](std::vector<Char> row){
        std::for_each(row.begin(), row.end(), [](Char val){
            std::cout << "Valore: " << val.getValue() << " Fractional: [";
            for(int i = 0; i < (int) val.getFractionalPosition().size(); i++){
                std::cout << val.getFractionalPosition()[i] << " ";
            }
            std::cout << "]" << std::endl;

            /*std::for_each(val.getFractionalPosition().begin(), val.getFractionalPosition().end(), [](auto i){
                qDebug() << i;
            });*/
        });
    });
}
