#include "Char.h"


Char::~Char()
{
}

void Char::setPosition(std::vector<int> x)
{
    this->position = x;
}

vector<int> Char::getPosition() {
    return position;
}

char Char::getValue() {
    return value;
}
