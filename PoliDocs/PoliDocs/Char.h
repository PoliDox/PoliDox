#pragma once
#include <vector>

class Char
{
private:
	int siteId;
	int counter;
	char value;
	std::vector<int> position;

public:
	Char(int siteId, int counter, char value) : siteId(siteId), counter(counter), value(value) {};
	~Char();
	void setPosition(std::vector<int> x);
};

