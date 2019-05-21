//
// Created by Gabriele Minì on 2019-05-21.
//

#ifndef POLIDOX_CHAR_H
#define POLIDOX_CHAR_H

#include <vector>

using namespace std;

class Char {
private:
    int siteId;
    int counter;
    char value;
    vector<int> position;

public:
    Char(int siteId, int counter, char value) : siteId(siteId), counter(counter), value(value) {};
    ~Char();
    void setPosition(std::vector<int> x);
    vector<int> getPosition();
    char getValue();
};


#endif //POLIDOX_CHAR_H
