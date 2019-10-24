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

//TODO: ATTENZIONE, DEFINIRE COSTRUTTORE DI COPIA E OPERATORE DI ASSEGNAZIONE!!!

class Char {

private:
    char value;
    std::vector<int> fractionalPosition;

public:
    Char(char value, std::vector<int>& fractionalPosition);
    Char(char value);
    std::vector<int> getFractionalPosition();
    void setFractionalPosition(std::vector<int>& fractionalPosition);
    char getValue() const;
    bool operator < (const Char& other) const;
    ~Char();

    int getSiteId(){return 0;}      //ATTENZIONE, METODO DA CANCELLARE!!!

    // used by the CLIENTcontroller to create the message to be sent to the server
    QJsonObject toJson() const;
    static Char fromJson(const QJsonObject& _JSONobj);
    //static Char fromJson2(const QString& stringReturnedFromDb);
};


#endif //POLIDOX_CHAR_H
