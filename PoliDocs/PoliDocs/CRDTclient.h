#pragma once
#include "CRDT.h"
#include "CLIENTcontroller.h"

class CRDTclient :
	public CRDT
{
private:
	CLIENTcontroller c_controller;


public:
	CRDTclient();
	~CRDTclient();
	void LocalInsert();
	void LocalDelete();

};

