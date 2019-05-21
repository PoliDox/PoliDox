#include "CRDTclient.h"



CRDTclient::CRDTclient()
{
}


CRDTclient::~CRDTclient()
{
}

void CRDTclient::LocalInsert(int row,int index, char value) {

    vector<int>fractionPos;

    srand(time(0));

    int pos,
            deep=0,
            leftMiss=0,
            rightMiss=0,
            equal=-1;

    _counter++;

    /* GESTIONE INSERIMENTO AL FONDO */
    if(_symbols.empty()||index==_symbols.size()) {

        /* i'm at the end and the vector is not empty */
        if(_symbols.size()>=1){

            Symbol left = _symbols.at(index - 1);
            int l = *(left.position.begin());       /* deferenzio iteratore ad inizio posizione */

            /* if the first element of the last symbol's position is the greates value ... */
            if(l==100){
                /* if it is the only element in position[], need to fraction */
                if(left.position.size()==1){
                    fractionPos.push_back(l);   /* copy base of the number */
                    pos = (rand() % 100) + 1;
                    fractionPos.push_back(pos); /* add a decimal value */
                }
                    /* otherwise copy until end-1 and find a new valure greater than end-1 */
                else {
                    auto it=_symbols.end()-1;

                    if(*(it->position.end()-1)==100){
                        fractionPos.assign(it->position.begin(),it->position.end());
                        pos = (rand() % 100) + 1;
                        fractionPos.push_back(pos);
                    }else{
                        fractionPos.assign(it->position.begin(),it->position.end()-1);
                        while((pos=(rand()%100)+1)<=*(it->position.end()-1)){}
                        fractionPos.push_back(pos);
                    }
                }
            }else{
                while((pos = (rand() % 100) + 1)<=l){}
                fractionPos.push_back(pos);}

        }else if(_symbols.size()==0){
            pos = rand() % 100+ 1;
            fractionPos.push_back(pos);
        }
        /* FINE GESTIONE INSERIMENTO AL FODNO */
    }else {
        Symbol left = _symbols.at(index - 1);
        Symbol right = _symbols.at(index);

        auto l_start=left.position.begin();
        auto r_start=right.position.begin();

        auto l_end=left.position.end();
        auto r_end=right.position.end();

        int l = *left.position.begin();
        int r = *right.position.begin();

        while (1) {
            if(r==l&&!leftMiss&&!rightMiss){
                equal=1;
                /* if number are equal the have been already fractioned*/
                deep++;
                /* start filling positions */
                fractionPos.push_back(l);
                /* right doesn't mattter, need to fill only position with left part */
                if(*(l_start+deep)!=*(l_end)&&*(r_start+deep)!=*(r_end)){
                    l=*(l_start+deep);
                    r=*(r_start+deep);
                    continue;
                }else if(*(l_start+deep)==*(l_end)&&*(r_start+deep)!=*(r_end)){
                    leftMiss=1;     /* element on the right has one extra decimal number than the one on the left */
                    r=*(r_start+deep);
                    l=0; // because of left_miss
                    continue;
                }else if(*(l_start+deep)!=*(l_end)&&*(r_start+deep)==*(r_end)){
                    rightMiss=1;     /* element on the right has one extra decimal number than the one on the left */
                    l=*(l_start+deep);
                    r=0; //because of right_miss
                    continue;
                }
                /* The case r==l and l.next=nul and r.next=null is not possible*/
            }

            /* if the range is equal to 1 need to fraction adding a decimal */
            if ((r - l)==1)  {
                equal=0;
                fractionPos.assign(l_start,l_end);
                if(leftMiss||rightMiss){
                    if(leftMiss==1){
                        if(r!=1)
                            while((pos = (rand() % 100) + 1)>=r){}
                        else{
                            fractionPos.push_back(0);
                            pos = (rand() % 100) + 1;
                        }
                    }
                    else if( rightMiss == 1)
                        while((pos = (rand() % 100) + 1)<=l){}

                    break;}
                else{
                    pos = rand() % (r - l)+ l+1;
                    break;
                }
            }
            else if(r-l!=1){
                if(r==l){
                    fractionPos.assign(l_start,l_end);
                    fractionPos.push_back(0);
                    pos = (rand() % 100) + 1;
                }else{
                    if(leftMiss||rightMiss)  /* copy fraction position only if has been fractioned */
                        fractionPos.assign(l_start,l_end);
                    while(1) {
                        pos = (rand() % 100) + 1;
                        if( pos<r && pos>l) /* don't need to fraction */
                            break;
                    }
                }
                break;
            }

        }
        fractionPos.push_back(pos);
    }
    vector<int> uniqueID;
    uniqueID.push_back(_siteId);
    uniqueID.push_back(_counter);
    Symbol newSymbol=Symbol(value,uniqueID,fractionPos);    //uniqueID as siteID + counter
    _symbols.insert(_symbols.begin()+index,newSymbol);

    //Message mex=Message("INSERT",newSymbol,this->_siteId);
    //this->_server.send(mex);

}

void CRDTclient::LocalDelete()
{
}
