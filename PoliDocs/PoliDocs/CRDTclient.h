#pragma once
#include "CRDT.h"
#include "CLIENTcontroller.h"
#include "Symbol.h"

#include <vector>


using namespace std;
class CRDTclient :
	public CRDT
{
private:
	CLIENTcontroller c_controller;
    vector<vector<Symbol>> _symbols;
    int _counter;
    int _siteID;


public:
	CRDTclient();
	~CRDTclient();
	void LocalInsert(int row,int index, Symbol value);
	void LocalDelete();

};

