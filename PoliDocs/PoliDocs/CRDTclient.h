#pragma once
#include "CRDT.h"
#include "CLIENTcontroller.h"
#include "Char.h"

#include <vector>
#include <iterator>
#include <ctime>
#include <cstdlib>

using namespace std;
class CRDTclient :
	public CRDT
{
private:
	CLIENTcontroller c_controller;
    static vector<vector<Char>> _symbols;
    static int _counter;
    static int _siteID;


public:
	CRDTclient();
	~CRDTclient();
	void LocalInsert(int row,int index, char value);
	void LocalDelete();

};

