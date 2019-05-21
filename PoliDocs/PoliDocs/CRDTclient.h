#pragma once
#include "CRDT.h"
#include "CLIENTcontroller.h"
#include "Char.h"

#include <vector>


using namespace std;
class CRDTclient :
	public CRDT
{
private:
	CLIENTcontroller c_controller;
    vector<vector<Char>> _symbols;


public:
	CRDTclient();
	~CRDTclient();
	void LocalInsert(int row,int index, Char value);
	void LocalDelete();

};

