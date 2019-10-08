//
// Created by Gabriele Minì on 2019-05-21.
//

#ifndef POLIDOX_CHAR_H
#define POLIDOX_CHAR_H

#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>

using namespace std;

class Char {
private:
    int siteId;
    int counter;
    char value;
    vector<int> position;

public:
    Char(int siteId, int counter, char value) : siteId(siteId), counter(counter), value(value){};
    ~Char();
    void setPosition(std::vector<int> x);
    vector<int> getPosition();
    char getValue();
    void setFractionalPosition(vector<int> fractionalPosition);
    vector<int> getFractionalPosition();

    // used by the CLIENTcontroller to create the message to be sent to the server
    QJsonDocument write(const QString &action );
    static Char read(const QString& _JSONstring);
};


#endif //POLIDOX_CHAR_H
