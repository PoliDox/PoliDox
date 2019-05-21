#include "Symbol.h"




Char::~Char()
{
}

void Symbol::setPosition(std::vector<int> x)
{
	this->position = x;
}

vector<int> Symbol::getPosition() {
    return position;
}

char Symbol::getValue() {
    return value;
}
