#include "CrdtClient.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QObject>
#include <future>

#define MAXNUM 100
//#define DEBUG_OUTPUT

CrdtClient::CrdtClient(ClientController *p_controller) : m_controller(p_controller) {
    //TODO il vettore di simboli inizialmente è vuoto??
    this->_symbols=std::vector<std::vector<Char>>(1);
    _siteID = 1; // ONLY FOR TESTING
}

bool existsPositionInVector(unsigned int position, std::vector<int> vector) {
    if(position < vector.size())
        return true;

        return false;
}

void mergeRows(std::vector<Char>& current,std::vector<Char>& next){

    current.erase(current.end()-1);
    current.insert(current.end(),next.begin(),next.end());
}

void splitRows(std::vector<std::vector<Char>>& matrix, std::vector<Char>& current,const unsigned int& row,const unsigned int& index){
    std::vector<Char> _VETT(current.begin()+index+1,current.end());
    matrix.insert(matrix.begin()+row+1,_VETT);
    //current.erase(current.begin()+index+1,current.end()); //WHY CURRENT INVALID AFTER INSERT, NOT AN ITERATOR. PROBABLY HEAP REALLOCATION ???
    matrix[row].erase(matrix[row].begin()+index+1,matrix[row].end());

}

void insertSymbolAt(std::vector<Char>&row,Char& symbol,const unsigned int index){
    row.insert(row.begin()+index,symbol);
}

void deleteRowAt(std::vector<std::vector<Char>>& matrix,unsigned int row){
    matrix.erase(matrix.begin()+row);
}

void inserRowAtEnd(std::vector<std::vector<Char>>& matrix,std::vector<Char>& row){
    matrix.push_back(row);
}
void searchEqualSymbol(std::vector<std::vector<Char>>& matrix,const Char& symbol,unsigned int& _row,unsigned int& _index,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit){

    _ROWhit=std::find_if(matrix.begin(),matrix.end(),[&](std::vector<Char>& row)->bool{
        _row++;
        _index=0;

        _INDEXhit=std::find_if(row.begin(),row.end(),[&](Char d_symbol)->bool{
            _index++;
            if(d_symbol.getFractionalPosition()==symbol.getFractionalPosition()){
                _index--;
                return true;
            }
            else
                return false;
        });

       if(_INDEXhit!=row.end()){
           _row--;
           return true;
       }
       else
           return false;
    });



}

void searchGreaterSymbol(std::vector<std::vector<Char>>& matrix,const Char& symbol,unsigned int& _row,unsigned int& _index,int& _LINECOUNTER,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit){

    _ROWhit = std::find_if(matrix.begin(), matrix.end(), [&](std::vector<Char>& row) -> bool{
            _index=0; //newline
            _row++;

            _INDEXhit = find_if(row.begin(), row.end(), [&](Char m_symbol) ->bool {
                _index++;
                if(_INDEXhit->getValue()=='\n')
                    _LINECOUNTER++;
                if(symbol.getFractionalPosition() < m_symbol.getFractionalPosition())
                    return true;
                else
                    return false;  
            });

            if(_INDEXhit!=row.end()){
                _index--;
                return true;
            }
            else
                return false;
    });
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

void CrdtClient::_toMatrix(unsigned int position,unsigned int* row,unsigned int* index){


    unsigned int row_counter=0,
                 _NEWLINE=0;

    while(1){

        if(position>this->_symbols[row_counter].size()){
            if((this->_symbols[row_counter].end()-1)->getValue()=='\n'){
                _NEWLINE++;
                 position-=this->_symbols[row_counter].size();
            }
        }
            else if(position==this->_symbols[row_counter].size()){
                if(this->_symbols[row_counter].size()!=0 && (this->_symbols[row_counter].end()-1)->getValue()=='\n'){
                    _NEWLINE++;
                     position-=this->_symbols[row_counter].size();
                }
                break;
            }
        else if(position<this->_symbols[row_counter].size())
                break;

         row_counter++;
    }

    *row=_NEWLINE;
    *index=position;



}

int CrdtClient::_toLinear(int row,int index){


    int position=0;

    std::vector<std::vector<Char>>::iterator _ROWit=this->_symbols.begin();
    std::vector<std::vector<Char>>::iterator _ROWend=_ROWit+row;

    while(_ROWit!=_ROWend){
        position+=_ROWit->size();
        _ROWit++;

    }

    position+=index;

    return position;

}


/* position Ã¨ il valore restituito dall'editor QT, va convertito in row e index della matrice CRDT */

void CrdtClient::localInsert(unsigned int position, char value) {

    unsigned int row=0,
                 index=0,
                 middleNewLine=0;

    unsigned long rowSize=0;

    this->_toMatrix(position,&row,&index);

    /* !!! SOLUTION TO 2ND BUG ON LOCAL INSERT !!! */
    if(row>=this->_symbols.size()){
        this->_symbols.insert(this->_symbols.begin() + (row), vector<Char>());
    }

#ifdef DEBUG_OUTPUT
    if(value=='\n')
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t\\n "<<"\tLINEAR POSITION " << position<< std::endl;
    else
        std::cout << "[LOCAL INSERT]@ [" << row << "][" << index << "]\t"<< value <<"\tLINEAR POSITION " << position << std::endl;
#endif

    Char symbolToInsert(this->_siteID, 0, value);

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
                symbolToInsert.setFractionalPosition(newFractionalPosition);
                this->_symbols[row].insert(this->_symbols[row].begin()+index, symbolToInsert);

            }else{
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

    emit onLocalInsert(symbolToInsert);
}



void CrdtClient::localDelete(unsigned int position){

    unsigned int row=0,
                 index=0,
                 _NROWS=this->_symbols.size();

    this->_toMatrix(position,&row,&index);

    Char _Dsymbol=this->_symbols[row][index];
    char _CHAR=_Dsymbol.getValue();

    if(_CHAR=='\n' &&_NROWS!=1 && row!=_NROWS-1){
        mergeRows(this->_symbols[row],this->_symbols[row+1]);
        deleteRowAt(this->_symbols,row+1);
    }else
        this->_symbols[row].erase(this->_symbols[row].begin()+index);

    if(this->_symbols[row].size()==0 && this->_symbols.size()>1) //editor empty=one empty row so don't clear last row
        deleteRowAt(this->_symbols,row);


    emit onLocalDelete(_Dsymbol);

#ifdef DEBUG_OUTPUT
    std::cout << "[LOCAL DELETE]@ [" << row << "] [" << index <<"]\t" << _Dsymbol.getValue() << std::endl;
#endif


}

/* ______________________________________________________________________________________
   IMPORTANTE!

   Nella prima find_if la lamba deve lavorare su un reference di std::vector<Char>&
   altrimenti nella copia ( per il passaggio per valore ) non viene riempita la position!

   //TODO aggiungere const al reference.
   ______________________________________________________________________________________     */
int CrdtClient::remoteInsert(Char symbol){

    std::vector<std::vector<Char>>::iterator _ROWhit;
    std::vector<Char>::iterator _INDEXhit;

    unsigned int _row=0,
                 _index=0;

    int _NOTFOUND=false,
        _NEWLINE=false,
        _LINECOUNTER=0;

    int _LINEARpos=0;

    char _CHAR=symbol.getValue();

    searchGreaterSymbol(this->_symbols,symbol,_row,_index,_LINECOUNTER,_ROWhit,_INDEXhit);

    _row--;

    if(this->_symbols.size()==1 && this->_symbols.begin()->size()==0){ //empty editor if size==1 al row.size==0
        insertSymbolAt(this->_symbols[0],symbol,0);
    }
    else if (_ROWhit != this->_symbols.end()){
        insertSymbolAt(this->_symbols[_row],symbol,_index);
        if(_CHAR=='\n')
             splitRows(this->_symbols,this->_symbols[_row],_row,_index);
    }
    else // se non ho trovato nulla, row e index non hanno significato, li setto dopo!
        {
             char _LASTCHAR=((this->_symbols.end()-1)->end()-1)->getValue();
            _NOTFOUND=true;

            if(_LASTCHAR=='\n'){
                _NEWLINE=true;
                std::vector<Char>_NEWROW(1,symbol);
                inserRowAtEnd(this->_symbols,_NEWROW);
            }
            else{
                insertSymbolAt(this->_symbols[this->_symbols.size()-1],symbol,(this->_symbols.end()-1)->size());
            }
        }

    if(_NOTFOUND && _NEWLINE){
       _row=this->_symbols.size()-1;
       _index=0;
    }


    _LINEARpos=_toLinear(_row,_index);


#ifdef DEBUG_OUTPUT

    if(_CHAR=='\n')
        std::cout << "[REMOTE INSERT]@ [" << _row << "][" << _index << "]\t\\n "<<"\tLINEAR POSITION " << _LINEARpos<< std::endl;
    else
        std::cout << "[REMOTE INSERT]@ [" << _row << "][" << _index <<"]\t" << _CHAR <<"\tLINEAR POSITION " << _LINEARpos<< std::endl;

#endif

    return _LINEARpos;


};

/* ______________________________________________________________________________________
   IMPORTANTE!
   Nella prima find_if la lamba deve lavorare su un reference di std::vector<Char>&
   altrimenti nella copia ( per il passaggio per valore ) non viene riempita la position!

   //TODO aggiungere const al reference.
   ______________________________________________________________________________________     */

int CrdtClient::remoteDelete(const Char& symbol) {

    std::vector<Char>::iterator _indexHIT;
    std::vector<std::vector<Char>>::iterator _rowHIT;

    unsigned int _row=0,
                 _index=0;

    int _LINEARpos=0;

    unsigned int _NROWS=this->_symbols.size();

    searchEqualSymbol(this->_symbols, symbol, _row, _index, _rowHIT,_indexHIT);

    if(_rowHIT!=this->_symbols.end()) { //if i found something
        char _CHAR=this->_symbols[_row][_index].getValue();

        if(_CHAR=='\n' && _NROWS!=1 && _row!=_NROWS-1){
         mergeRows(this->_symbols[_row],this->_symbols[_row+1]);
         deleteRowAt(this->_symbols,_row+1);
        }
        else
            _rowHIT->erase(_indexHIT);

         if(this->_symbols[_row].size()==0 && this->_symbols.size()>1) //editor empty=one empty row so don't clear last row
             deleteRowAt(this->_symbols,_row);
    }
    else
        throw std::string("REMOTE DELETE FAILED, CHAR NOT FOUND!");


    _LINEARpos=_toLinear(_row,_index);


#ifdef DEBUG_OUTPUT
    std::cout << "[REMOTE DELETE]@ [" << _row << "] [" << _index <<"]\t" << symbol.getValue() <<"\tLINEAR POSITION " << _LINEARpos<< std::endl;
#endif

    return _LINEARpos;
}


int CrdtClient::getSiteId() {
    return this->_siteID;
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

