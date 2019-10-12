#include "CrdtClient.h"
#include <iostream>
#include <string>
#include <QDebug>
#include <QObject>

#define MAXNUM 100


CrdtClient::CrdtClient(ClientController *p_controller) : m_controller(p_controller) {
    //TODO il vettore di simboli inizialmente è vuoto??
    this->_symbols=std::vector<std::vector<Char>>(1);
    this->_counter = 0; //TODO ?? siamo sicuri che sia inizializzato a zero?

    _siteID = 1; // ONLY FOR TESTING
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


    int row_counter=0,
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

void CrdtClient::localInsert(int position, char value) {

    int row=0,
        index=0;

    this->_toMatrix(position,&row,&index);

    /* !!! SOLUTION TO 2ND BUG ON LOCAL INSERT !!! */
    if(row>=this->_symbols.size()){
        this->_symbols.insert(this->_symbols.begin() + (row), vector<Char>());
    }

    if(value=='\n')
        std::cout << "[LOCAL INSERT]@ " << row << " " << index << " \\n "<< std::endl;
    else
        std::cout << "[LOCAL INSERT]@ " << row << " " << index << " \ "<< value << std::endl;

    Char symbolToInsert(this->_siteID, 0, value);

    /* no more symbolsSyze needed, it's the row size */
    //int symbolsSize = this->_symbols[row]->size();
    unsigned long rowSize=this->_symbols[row].size();


    int middleNewLine=0;

    if(value=='\n') {
        /*  !!!!!BUG SU LOCAL INSERT!!!!!
            if (index == this->_symbols[row].size() - 1)
            this->_symbols.insert(this->_symbols.begin() + (row + 1), vector<Char>());  /* allocate pointer for new line */

        /*if (index == this->_symbols[row].size())
           this->_symbols.insert(this->_symbols.begin() + (row + 1), vector<Char>()); //NON NECESSARIO DOPO FIX SECONDO BUG
        else{
            middleNewLine=1;
        }*/

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

    /*std::cout << "Added symbol " << value << " (" << (int)value << ") at position: [";
    for(int i=0;i<symbolToInsert.getFractionalPosition().size();i++)
        std::cout << symbolToInsert.getFractionalPosition()[i] <<" ";
    std::cout<<"]"<<std::endl;*/

    //printDebugChars();


    emit onLocalInsert(symbolToInsert);
}



void CrdtClient::localDelete(int position){

    int row=0,
        index=0;

    this->_toMatrix(position,&row,&index);


    Char _Dsymbol=this->_symbols[row][index];


    if(row>0&&index==this->_symbols[row-1].size()+1){

        if((this->_symbols[row-1].end()-1)->getValue()=='\n'){
            this->_symbols[row].erase(this->_symbols[row].end()-1);
            this->_symbols[row-1].insert(this->_symbols[row-1].end(),this->_symbols[row].begin(),this->_symbols[row].end());
            this->_symbols.erase(this->_symbols.begin()+row);
        }else{
            this->_symbols[row].erase(this->_symbols[row].begin()+index);
            if(this->_symbols[row].size()==0)
                this->_symbols.erase(this->_symbols.begin()+row);
        }

    }else{
        this->_symbols[row].erase(this->_symbols[row].begin()+index);

        if(_Dsymbol.getValue()=='\n'&& (row+1<this->_symbols.size())){
            this->_symbols[row].insert(this->_symbols[row].end(),this->_symbols[row+1].begin(),this->_symbols[row+1].end());
            this->_symbols.erase(this->_symbols.begin()+row+1);
        }
        if(this->_symbols[row].size()==0)
            this->_symbols.erase(this->_symbols.begin()+row);
    }

    emit onLocalDelete(_Dsymbol);

    std::cout << "[LOCAL DELETE]@ " << row << " " << index << std::endl;


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

    int _row=0,
        _index=0,
        _NOTFOUND=false,
        _NEWLINE=false;

    int _LINEARpos=0;

    char _CHAR=symbol.getValue();

    _ROWhit = std::find_if(this->_symbols.begin(), this->_symbols.end(), [&](std::vector<Char>& row) -> bool{

            _index=0; //newline
            _row++;

            _INDEXhit = find_if(row.begin(), row.end(), [&](Char m_symbol) ->bool {

                _index++;

                return symbol.getFractionalPosition() < m_symbol.getFractionalPosition();

            });

            if(_INDEXhit!=row.end()){
                _index--;               /* se l'inserimento non avviene alla fine si fa un confronto in più */
                return true;
            }
            else{
                return false;
            }

    });

    /* se editor vuoto */
    if(this->_symbols.size()==1 && this->_symbols.begin()->size()==0){

        this->_symbols.begin()->push_back(symbol);
    }
    else if (_ROWhit != this->_symbols.end()  ) {

        if(_CHAR=='\n'){

             std::vector<Char> _VETT(_INDEXhit,_ROWhit->end());

             this->_symbols.insert(_ROWhit+1, _VETT);
              (this->_symbols.begin()+_row-1)->erase(_INDEXhit, (this->_symbols.begin()+_row-1)->end());


        }

        (this->_symbols.begin()+_row-1)->insert((this->_symbols.begin()+_row-1)->begin()+_index,symbol);

    } else

    /*  if anything bigger than the symbol to inser has been found i need to check if
     *  the last character of the last line is '\n'. If Yes, create a new row otherwise
     *  insert the symbol as the last character of the last row.                    */

        {

        _NOTFOUND=true;

        if(((this->_symbols.end()-1)->end()-1)->getValue()=='\n'){
            _NEWLINE=true;
            this->_symbols.push_back(std::vector<Char>(1,symbol));
        }
        else
            this->_symbols[this->_symbols.size()-1].push_back(symbol);

    }

    if(!_NOTFOUND||!_NEWLINE)
            _LINEARpos=_toLinear(_row-1,_index);
    else
            _LINEARpos=_toLinear(this->_symbols.size()-1,0);





    if(_CHAR=='\n')
        std::cout << "[REMOTE INSERT]@ " << _row-1 << " " << _index << " \\n "<< std::endl;
    else
        std::cout << "[REMOTE INSERT]@ " << _row-1 << " " << _index <<" " << _CHAR << std::endl;
    //emit this->onRemoteInsert(_LINEARpos,symbol.getValue());


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

    int _row=-1,
        _index=-1;

    int _LINEARpos=0;

    _rowHIT=std::find_if(this->_symbols.begin(),this->_symbols.end(),[&](std::vector<Char>& row)->bool{

        _row++;
        _index=0;

        _indexHIT=std::find_if(row.begin(),row.end(),[&](Char d_symbol)->bool{

            _index++;

            if(d_symbol.getFractionalPosition()==symbol.getFractionalPosition())
                return true;
            else
                return false;

        });


       if(_indexHIT!=row.end()){
           return true;
       }
       else
           return false;

    });


    if(_rowHIT!=this->_symbols.end()){

    if(_row>0&&(_index-1)==this->_symbols[_row-1].size()){

        if((_rowHIT->begin()+_index-1)->getValue()=='\n'){
             this->_symbols[_row].erase(this->_symbols[_row].end()-1);
             this->_symbols[_row].insert(this->_symbols[_row].end(),this->_symbols[_row+1].begin(),this->_symbols[_row+1].end());
             this->_symbols.erase(this->_symbols.begin()+_row+1);
        }else{
            _rowHIT->erase(_indexHIT);
            if(this->_symbols[_row].size()==0)
                this->_symbols.erase(this->_symbols.begin()+_row);
        }
    }else
            _rowHIT->erase(_indexHIT);
            if(this->_symbols[_row].size()==0)
                this->_symbols.erase(this->_symbols.begin()+_row);
    }
    else
        throw std::string("REMOTE DELETE FAILED, CHAR NOT FOUND!");

    if(_index!=0)
        _LINEARpos=_toLinear(_row,_index-1);
    else
        _LINEARpos=_toLinear(_row,0);

    std::cout << "[REMOTE DELETE]@ " << _row << " " << _index-1 << std::endl;

    return _LINEARpos;
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

