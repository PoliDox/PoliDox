// PoliDocs.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include "CRDTclient.h"

int main()
{
    std::cout << "Hello World!\n";

    CRDTclient *test= new CRDTclient();
    char a='a';
    test->LocalInsert(0,0,a);
    test->LocalInsert(0,1,a);
    test->LocalInsert(0,2,a);
    test->LocalInsert(0,3,a);
    test->LocalInsert(0,2,a);

}
