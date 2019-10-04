#ifndef SYMBOL_H
#define SYMBOL_H


//
// Created by federico on 06/05/19.
//

#include <vector>
#include <cstdint>

/* modella un singolo carattere
 * all'interno dell'editor */
class Symbol {
private:
    char value;
    /* l'id univoco(globalmente) del Symbol è dato
     * dalla concantenazione di idOfClient e counterOfClient */
    uint32_t idOfClient;
    uint32_t counterOfClient;
    /* posizione del symbol all'interno del file,
     * creato tramite l'algoritmo CRDT */
    std::vector<int> fractionalPosition;
public:
    Symbol(char value, uint32_t idOfClient, uint32_t counterOfClient);

    void setFractionalPosition(std::vector<int> fractionalPosition);
    std::vector<int> getFractionalPosition();
    std::vector<int> getUniqueId();
    uint32_t getIdOfClient();
    uint32_t getCounterOfClient();
    char getValue();
};



#endif // SYMBOL_H
