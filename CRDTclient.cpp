//
// Created by federico on 06/05/19.
//

#include "CRDTclient.h"
#include <iostream>
#include <string>
#include <QDebug>

#define MAXNUM 100

bool existsPositionInVector(int position, std::vector<int> vector) {
    if(position < vector.size())
        return true;
    return false;
}


/* assumo che sia preceding che following
 * abbiamo almeno un elemento    */
std::vector<int> createMiddleFractionalNumber(std::vector<int> preceding, std::vector<int> following){
    std::vector<int> middle;

    int moreSmallVector, precedingSize=preceding.size(), followingSize=following.size();
    if(precedingSize < followingSize)
        moreSmallVector = precedingSize;
    else
        moreSmallVector = followingSize;

    int i;
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
                //qui terminerò sicuro, devo solo trovare un numero più
                //grande di preceding da poter inserire
                if( existsPositionInVector(i+1, preceding) ) {
                    for(int j=i+1; j<precedingSize; j++) {
                        if (preceding[j] <= (MAXNUM - 2)) {
                            int middleElement = (MAXNUM - preceding[j]) / 2;
                            middleElement = middleElement + preceding[j];
                            middle.push_back(middleElement);
                            return middle;
                        } else {
                            //copia preceding finché non trovi
                            //un elemento i-esimo più grande da poter inserire.
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

    //se arrivo qui è perché sicuramente
    //preceding è finito e following ancora no!!
    //TODO da confermare se è vero

    bool precedingIsFinish = i==precedingSize;
    bool followingIsFinish = i==followingSize;

    if( precedingIsFinish && followingIsFinish){
        std::cout << "ERRORE\n"; //NON DOVREI MIA ENTRARE QUI
        exit(10);  //10: numero a caso
    }

    if(precedingIsFinish){
        //preceding è finito e following no
        for(i; i<followingSize; i++) {
            if (following[i] == 0) {  //può essere sia 1 che zero
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

/* 1- costruisco un symbol e genero la sua fractionalPosition */
void CRDTclient::localInsert(int index, char value) {

    Symbol symbolToInsert(value, this->getSiteId(), this->getCounterAndIncrement());
    int symbolsSize = this->symbols.size();
    if(index == 0){
        if(symbolsSize == 0){
            //è il primo elemento che inserisco
            std::vector<int> firstElem{MAXNUM/2};

            symbolToInsert.setFractionalPosition(firstElem);
            this->symbols.insert(this->symbols.begin()+index, symbolToInsert);
        }
        else {
            //ho degli elementi dopo di me
            std::vector<int> followingFractionalNumber = this->symbols[0].getFractionalPosition();
            std::vector<int> fakeVector{0};
            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(fakeVector, followingFractionalNumber);

            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->symbols.insert(this->symbols.begin()+index, symbolToInsert);
        }
    }
    else {
        //sicuramente avrò un simbolo prima di me
        if(index >= symbolsSize){  //TODO controllare bene il confronto
            //non ho nessuno dopo di me
            index = symbolsSize;  //inserisco in append
            std::vector<int> precedingFractionalNumber = this->symbols[index-1].getFractionalPosition();
            std::vector<int> fakeVector{MAXNUM};

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, fakeVector);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->symbols.insert(this->symbols.begin()+index, symbolToInsert);
        }
        else {
            //sono fra due simboli
            std::vector<int> precedingFractionalNumber = this->symbols[index-1].getFractionalPosition();
            std::vector<int> followingFractionalNumber = this->symbols[index].getFractionalPosition();

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->symbols.insert(this->symbols.begin()+index, symbolToInsert);
        }
    }


    // only for debug purposes
    printSymbols();


}

void CRDTclient::localErase(int index) {
    /* TODO -controllare se effettivamente il Symbol(l'oggetto) viene
     * TODO cancellato. Controllare anche nelle altre parti del codice
     * TODO se ci sono memory leak o cose simili. */
    Symbol symbolErased = this->symbols[index];
    this->symbols.erase(this->symbols.begin()+index);
}

/*
void CRDTclient::process() {

    if(isInserimento){
        int i;
        Symbol symbolToInsert = messageToProcess.getSymbolToSend();
        for(i=0; i<this->symbols.size(); i++){
            Symbol currentSymbol = this->symbols[i];
            if(currentSymbol.getFractionalPosition() == symbolToInsert.getFractionalPosition()){
                //se hanno la stessa posizione frazionaria,
                //scelgo di dare sempre la precedenza al symbolo
                //il cui siteId è minore
                if(this->getSiteId() < symbolToInsert.getIdOfClient())
                    this->symbols.insert(this->symbols.begin()+i, symbolToInsert);
                else
                    this->symbols.insert(this->symbols.begin()+i+1, symbolToInsert);
                return;
            }

            if(currentSymbol.getFractionalPosition() > symbolToInsert.getFractionalPosition()){
                this->symbols.insert(this->symbols.begin()+i, symbolToInsert);
                return;
            }
        }

        this->symbols.insert(this->symbols.begin()+i, messageToProcess.getSymbolToSend());
        return;
    }
    else {
        //è una cancellazione
        int i;
        Symbol symbolToErase = messageToProcess.getSymbolToSend();
        for(i=0; i<this->symbols.size(); i++){

            Symbol currentSymbol = this->symbols[i];

            if(currentSymbol.getFractionalPosition() == symbolToErase.getFractionalPosition()
            && currentSymbol.getUniqueId() == symbolToErase.getUniqueId()){
                this->symbols.erase(this->symbols.begin()+i);
                return;
            }

            if(currentSymbol.getFractionalPosition() > symbolToErase.getFractionalPosition())
                return;
        }

    }

}
*/

/* ricostruisce la sequenza di caratteri
 * contenuta nell'editor */
std::string CRDTclient::toString() {
    std::string carSequence;
    int i;
    for(i=0; i<this->symbols.size(); i++){
        carSequence.insert(i, 1, this->symbols[i].getValue());
    }
    return carSequence;
}


void CRDTclient::printSymbols()
{
    qDebug() << "-------------";
    for (int i=0; i<symbols.size(); i++)
    {
        std::cout << "'" << symbols.at(i).getValue() << "' |";
        std::vector<int> l_pos = symbols.at(i).getFractionalPosition();
        for (int j=0; j<l_pos.size(); j++)
        {
            std::cout << " " << l_pos.at(j);
        }
        std::cout << std::endl;
    }
}

CRDTclient::CRDTclient() {
    //TODO il vettore di simboli inizialmente è vuoto??
    this->counter = 0; //TODO ?? siamo sicuri che sia inizializzato a zero?
}

int CRDTclient::getSiteId() {
    return this->siteId;
}

int CRDTclient::getCounterAndIncrement() {
    int oldCounter = this->counter;
    this->counter++;
    return oldCounter;
}

std::vector<Symbol> CRDTclient::getSymbols(){
    return this->symbols;
}

int CRDTclient::getCounter() {
    return this->counter;
}
