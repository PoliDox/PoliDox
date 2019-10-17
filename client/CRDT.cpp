#include "CRDT.h"
#include <future>

CRDT::CRDT(){
    this->_symbols=std::vector<std::vector<Char>>(1);
}

void CRDT::mergeRows(std::vector<Char>& current,std::vector<Char>& next){

    current.erase(current.end()-1);
    current.insert(current.end(),next.begin(),next.end());
}

void CRDT::splitRows(std::vector<Char>& current,const unsigned int& row,const unsigned int& index){
    std::vector<Char> _VETT(current.begin()+index+1,current.end());
    this->_symbols.insert(this->_symbols.begin()+row+1,_VETT);
    //current.erase(current.begin()+index+1,current.end()); //WHY CURRENT INVALID AFTER INSERT, NOT AN ITERATOR. PROBABLY HEAP REALLOCATION ???
    this->_symbols[row].erase(this->_symbols[row].begin()+index+1,this->_symbols[row].end());

}

void CRDT::insertSymbolAt(std::vector<Char>&row,Char& symbol,const unsigned int index){
    row.insert(row.begin()+index,symbol);
}

void CRDT::deleteRowAt(unsigned int row){
    this->_symbols.erase(this->_symbols.begin()+row);
}

void CRDT::inserRowAtEnd(std::vector<Char>& row){
    this->_symbols.push_back(row);
}

void CRDT::deleteSymbolAt(std::vector<Char>& row,unsigned int index){
    row.erase(row.begin()+index);
}

void CRDT::searchEqualSymbol(const Char& symbol,unsigned int& _row,unsigned int& _index,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit){

    _ROWhit=std::find_if(this->_symbols.begin(),this->_symbols.end(),[&](std::vector<Char>& row)->bool{
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

void CRDT::searchGreaterSymbol(const Char& symbol,unsigned int& _row,unsigned int& _index,int& _LINECOUNTER,std::vector<std::vector<Char>>::iterator& _ROWhit,std::vector<Char>::iterator& _INDEXhit){

    _ROWhit = std::find_if(this->_symbols.begin(), this->_symbols.end(), [&](std::vector<Char>& row) -> bool{
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


void CRDT::_toMatrix(unsigned int position,unsigned int* row,unsigned int* index){


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

int CRDT::_toLinear(int row,int index){


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



int CRDT::remoteInsert(Char symbol){

    std::vector<std::vector<Char>>::iterator _ROWhit;
    std::vector<Char>::iterator _INDEXhit;

    unsigned int _row=0,
                 _index=0;

    int _NOTFOUND=false,
        _NEWLINE=false,
        _LINECOUNTER=0;

    int _LINEARpos=0;

    char _CHAR=symbol.getValue();

    searchGreaterSymbol(symbol,_row,_index,_LINECOUNTER,_ROWhit,_INDEXhit);

    _row--;

    if(this->_symbols.size()==1 && this->_symbols.begin()->size()==0){ //empty editor if size==1 al row.size==0
        insertSymbolAt(this->_symbols[0],symbol,0);
    }
    else if (_ROWhit != this->_symbols.end()){
        insertSymbolAt(this->_symbols[_row],symbol,_index);
        if(_CHAR=='\n')
             splitRows(this->_symbols[_row],_row,_index);
    }
    else // se non ho trovato nulla, row e index non hanno significato, li setto dopo!
        {
             char _LASTCHAR=((this->_symbols.end()-1)->end()-1)->getValue();
            _NOTFOUND=true;

            if(_LASTCHAR=='\n'){
                _NEWLINE=true;
                std::vector<Char>_NEWROW(1,symbol);
                inserRowAtEnd(_NEWROW);
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


int CRDT::remoteDelete(const Char& symbol) {

    std::vector<Char>::iterator _indexHIT;
    std::vector<std::vector<Char>>::iterator _rowHIT;

    unsigned int _row=0,
                 _index=0;

    int _LINEARpos=0;

    unsigned int _NROWS=this->_symbols.size();

    searchEqualSymbol(symbol, _row, _index, _rowHIT,_indexHIT);

    if(_rowHIT!=this->_symbols.end()) { //if i found something
        char _CHAR=this->_symbols[_row][_index].getValue();

        if(_CHAR=='\n' && _NROWS!=1 && _row!=_NROWS-1){
         mergeRows(this->_symbols[_row],this->_symbols[_row+1]);
         deleteRowAt(_row+1);
        }
        else
            _rowHIT->erase(_indexHIT);

         if(this->_symbols[_row].size()==0 && this->_symbols.size()>1) //editor empty=one empty row so don't clear last row
             deleteRowAt(_row);
    }
    else
        throw std::string("REMOTE DELETE FAILED, CHAR NOT FOUND!");


    _LINEARpos=_toLinear(_row,_index);


#ifdef DEBUG_OUTPUT
    std::cout << "[REMOTE DELETE]@ [" << _row << "] [" << _index <<"]\t" << symbol.getValue() <<"\tLINEAR POSITION " << _LINEARpos<< std::endl;
#endif

    return _LINEARpos;
}



