//
// Created by federico on 06/05/19.
//

#include "symbol.h"

Symbol::Symbol(char value, uint32_t idOfClient, uint32_t counterOfClient)
: value(value), idOfClient(idOfClient), counterOfClient(counterOfClient){
    //nient'altro da fare
}

uint32_t Symbol::getIdOfClient() {
    return this->idOfClient;
}

uint32_t Symbol::getCounterOfClient() {
    return this->counterOfClient;
}

char Symbol::getValue() {
    return this->value;
}

void Symbol::setFractionalPosition(std::vector<int> fractionalPosition) {
    this->fractionalPosition = fractionalPosition;
}

std::vector<int> Symbol::getFractionalPosition() {
    return this->fractionalPosition;
}

/* concatena l'id e il counter del client per ottenere
 * l'id univoco del simbolo.
 * Ritorna un vettore di due elementi */
std::vector<int> Symbol::getUniqueId() {
    std::vector<int> uniqueId;
    uniqueId.push_back(this->idOfClient);
    uniqueId.push_back(this->counterOfClient);
    return uniqueId;
}
