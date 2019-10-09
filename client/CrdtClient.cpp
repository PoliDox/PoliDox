#include "CrdtClient.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QObject>

#define MAXNUM 100

CrdtClient::CrdtClient(ClientController *p_controller) : m_controller(p_controller) {
    //TODO il vettore di simboli inizialmente Ã¨ vuoto??
    this->_symbols=std::vector<std::vector<Char>>(1);
    this->_counter = 0; //TODO ?? siamo sicuri che sia inizializzato a zero?
}

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
                //qui terminerÃ² sicuro, devo solo trovare un numero piÃ¹
                //grande di preceding da poter inserire
                if( existsPositionInVector(i+1, preceding) ) {
                    for(int j=i+1; j<precedingSize; j++) {
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
        for(i; i<followingSize; i++) {
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

void CrdtClient::_toMatrix(int position,int* row,int* index){

    int totalLenght=0,
        row_counter=0;

    auto it=std::find_if(this->_symbols.begin(),this->_symbols.end(),[&](std::vector<Char> matrix_row) -> bool{


        if(position < matrix_row.size())
            return true;
        else{
        totalLenght+=matrix_row.size();
            return false;
        }
    });

    *row=row_counter;

    if(row_counter>0)
        *index=position-totalLenght;
    else
        *index=position;    //se sono alla prima riga parto da totalLenght=0
}


/* position Ã¨ il valore restituito dall'editor QT, va convertito in row e index della matrice CRDT */

void CrdtClient::localInsert(int position, char value) {

   //TODO fare conversione position -> row,index
    int row=0,
        index=0;

    this->_toMatrix(position,&row,&index);

    Char symbolToInsert(this->_siteID, 0, value);

    /* no more symbolsSyze needed, it's the row size */
    //int symbolsSize = this->_symbols[row]->size();
    unsigned long rowSize=this->_symbols[row].size();


    int middleNewLine=0;

    if(value=='\n') {
        if (index == this->_symbols[row].size() - 1)
            this->_symbols.insert(this->_symbols.begin() + (row + 1), vector<Char>());  /* allocate pointer for new line */
        else{
            middleNewLine=1;
        }
    }



    if(index == 0){
        if(rowSize == 0){
            std::vector<int> precedingFractionalNumber;
            //Ã¨ il primo elemento che inserisco
            std::vector<int> firstElem{MAXNUM/2};
            std::vector<int> fakeVector{MAXNUM};
            if(row!=0){
                precedingFractionalNumber = this->_symbols[row - 1].at(this->_symbols[row - 1].size() - 1).getFractionalPosition();
                std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber,fakeVector);
                symbolToInsert.setFractionalPosition(newFractionalPosition);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);

            }else{
            symbolToInsert.setFractionalPosition(firstElem);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);}
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
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
    }
    else {
        //sicuramente avrÃ² un simbolo prima di me
        if(index >= rowSize){  //TODO controllare bene il confronto
            //non ho nessuno dopo di me
            index = rowSize;  //inserisco in append
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> fakeVector{MAXNUM};

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, fakeVector);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
        else {
            //sono fra due simboli
            std::vector<int> precedingFractionalNumber = this->_symbols[row].at(index-1).getFractionalPosition();
            std::vector<int> followingFractionalNumber = this->_symbols[row].at(index).getFractionalPosition();

            std::vector<int> newFractionalPosition = createMiddleFractionalNumber(precedingFractionalNumber, followingFractionalNumber);
            symbolToInsert.setFractionalPosition(newFractionalPosition);
            this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);
        }
    }

    if(middleNewLine==1) {
        this->_symbols.insert(this->_symbols.begin() + (row + 1),
                               vector<Char>(this->_symbols[row].begin() + index + 1, this->_symbols[row].end()));
        this->_symbols[row].erase(this->_symbols[row].begin() + index + 1, this->_symbols[row].end());

    }

    std::cout<<"Added symbol "<< value <<" position: [";
    for(int i=0;i<symbolToInsert.getFractionalPosition().size();i++)
        std::cout << symbolToInsert.getFractionalPosition()[i] <<" ";
    std::cout<<"]"<<std::endl;

    /* Numero frazionario generato e simbolo inserito.
     * Generare ora il Message da spedire */
    //Message messageToSend(true, symbolToInsert, this);
    //this->server.send(messageToSend);
    emit onLocalInsert(symbolToInsert);
}



void CrdtClient::localDelete(int position){

    int row=0,
        index=0;

    printDebugChars();
    std::cout <<"____________________________"<<std::endl;
    Char _Dsymbol=this->_symbols[row][index];

    this->_toMatrix(position,&row,&index);

    this->_symbols[row].erase(this->_symbols[row].begin()+index);

    printDebugChars();
    emit onLocalDelete(_Dsymbol);


}

void CrdtClient::remoteInsert(Char symbol){

    int  _row=0,
         index=0;


    auto it1=std::find_if(this->_symbols.begin(),this->_symbols.end(),[&](std::vector<Char> row){

            _row++;

            auto it2=find_if(row.begin(),row.end(),[&](Char m_symbol){

            index++;

            return symbol.getFractionalPosition()<m_symbol.getFractionalPosition();

            });

            if(it2!=row.end()){

                this->_symbols[_row-1].insert(it2,symbol);

                 if(symbol.getValue()=='\n'){

                     this->_symbols.insert(this->_symbols.begin() + (_row),
                                            vector<Char>(this->_symbols[_row-1].begin() + index + 1, this->_symbols[_row-1].end()));
                     this->_symbols[_row-1].erase(this->_symbols[_row-1].begin() + index + 1, this->_symbols[_row-1].end());

                 }


                }
                return true;
    });

    if(it1==this->_symbols.end())
        this->_symbols.push_back(std::vector<Char>(1,symbol));


};

void CrdtClient::remoteDelete(const Char& symbol){

    std::vector<Char>::iterator _indexHIT;
    std::vector<std::vector<Char>>::iterator _rowHIT;

    _rowHIT=std::find_if(this->_symbols.begin(),this->_symbols.end(),[&](std::vector<Char> row)->bool{

        _indexHIT=std::find_if(row.begin(),row.end(),[&](Char d_symbol)->bool{

            if(d_symbol.getFractionalPosition()==symbol.getFractionalPosition())
                return true;
            else
                return false;

        });

       if(_indexHIT!=row.end())
           return true;
       else
           return false;

    });

    if(_rowHIT!=this->_symbols.end())
         _rowHIT->erase(_indexHIT);

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


int CrdtClient::getSiteId() {
    return this->_siteID;
}

int CrdtClient::getCounterAndIncrement() {
    int oldCounter = this->_counter;
    this->_counter++;
    return oldCounter;
}

int CrdtClient::getCounter() {
    return this->_counter;
}

