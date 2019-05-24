// PoliDocs.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include "CRDTclient.h"

int main()
{
    std::cout << "Hello World!\n";

    CRDTclient *test= new CRDTclient();
    char a='a',
         end='\n';
    test->LocalInsert(0,0,a);
    test->LocalInsert(0,1,a);
    test->LocalInsert(0,2,a);
    test->LocalInsert(0,3,a);
    test->LocalInsert(0,4,end);

    test->LocalInsert(1,0,a);
    test->LocalInsert(1,1,a);
    test->LocalInsert(1,2,a);
    test->LocalInsert(1,3,a);
    test->LocalInsert(1,1,end);
    test->LocalInsert(2,0,a);
    test->LocalInsert(2,1,a);
    test->LocalInsert(2,2,a);

    test->CRDTprintText();

}
