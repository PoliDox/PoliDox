#pragma once
#include "CRDT.h"
#include "CLIENTcontroller.h"

class CRDTclient :
	public CRDT
{
private:
	CLIENTcontroller c_controller;
    vector<vector<Symbol>> _symbols;


public:
	CRDTclient();
	~CRDTclient();
	void LocalInsert();
	void LocalDelete();

};

