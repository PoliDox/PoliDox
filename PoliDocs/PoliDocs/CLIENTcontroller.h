#pragma once
#include "EDITOR.h"
#include "Account.h"

class CLIENTcontroller
{
private:
	int siteCounter;
	EDITOR editor;
	Account account;

public:
	CLIENTcontroller();
	~CLIENTcontroller();
	void handleOperation();
};

