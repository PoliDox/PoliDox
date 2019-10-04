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

void Char::setFractionalPosition(vector<int> fractionalPosition) {
        this->position = fractionalPosition;
}

vector<int> Char::getFractionalPosition() {
    return position;
}
