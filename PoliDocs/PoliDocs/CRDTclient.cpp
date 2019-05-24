#include "CRDTclient.h"

#define MAXVAL 100


CRDTclient::CRDTclient()
{
   /* for(int i=0;i<100;i++){
        this->_symbols.push_back(new vector<Char>());
    }*/

    this->_symbols.push_back(new vector<Char>());
}


CRDTclient::~CRDTclient()
{
}

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
void CRDTclient::LocalInsert(int row, int index, char value) {
    Char symbolToInsert(value, this->_siteID, this->_counter);
    int symbolsSize = this->_symbols[row]->size();
    int middleNewLine=0;

    if(value=='\n') {
        if (index == this->_symbols[row]->size() - 1)
            this->_symbols.insert(this->_symbols.begin() + (row + 1),new vector<Char>());  /* allocate pointer for new line */
        else{
            middleNewLine=1;
        }
    }



    if(index == 0){
        if(symbolsSize == 0){
            //è il primo elemento che inserisco
            std::vector<int> firstElem{MAXNUM/2};

            symbolToInsert.setFractionalPosition(firstElem);
            this->_symbols[row]->insert(this->_symbols[row]->begin()+index, symbolToInsert);
        }
        else {
            //ho degli elementi dopo di me
            std::vector<int> followingFractionalNumber = this->_symbols[0]->at(0).getFractionalPosition();
            std::vector<int> fakeVector{0};
            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(fakeVector, followingFractionalNumber);

            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row]->insert(this->_symbols[row]->begin()+index, symbolToInsert);
        }
    }
    else {
        //sicuramente avrò un simbolo prima di me
        if(index >= symbolsSize){  //TODO controllare bene il confronto
            //non ho nessuno dopo di me
            index = symbolsSize;  //inserisco in append
            std::vector<int> precedingFractionalNumber = this->_symbols[row]->at(index-1).getFractionalPosition();
            std::vector<int> fakeVector{MAXNUM};

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, fakeVector);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row]->insert(this->_symbols[row]->begin()+index, symbolToInsert);
        }
        else {
            //sono fra due simboli
            std::vector<int> precedingFractionalNumber = this->_symbols[row]->at(index-1).getFractionalPosition();
            std::vector<int> followingFractionalNumber = this->_symbols[row]->at(index).getFractionalPosition();

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row]->insert(this->_symbols[row]->begin()+index, symbolToInsert);
        }
    }

    if(middleNewLine==1) {
       /* this->_symbols.insert(this->_symbols.begin() + (row + 1),
                              new vector<Char>(this->_symbols[row]->begin() + index + 1, this->_symbols[row]->end())); */
        this->_symbols[row]->resize(index + 1);
    }


    /* Numero frazionario generato e simbolo inserito.
     * Generare ora il Message da spedire */
    //Message messageToSend(true, symbolToInsert, this);
    //this->server.send(messageToSend);
}
void CRDTclient::LocalDelete()
{
}

void CRDTclient::CRDTprintText(){
    for(auto x : _symbols){
        for(auto y : *x){
            for(auto k : y.getPosition())
                  cout << k << " ";
             cout<<endl;
        }
        cout<< "_____NUOVA_RIGA______\n";
    }
}