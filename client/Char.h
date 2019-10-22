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



class Char {
private:
    double siteId;
    int counter;
    char value;
    std::vector<int> position;

public:
    Char(double siteId, int counter, char value);
    ~Char();
    double getSiteId() const;
    void setPosition(std::vector<int>& x);
    std::vector<int> getPosition();
    char getValue() const;
    void setFractionalPosition(std::vector<int>& fractionalPosition);
    std::vector<int> getFractionalPosition();

    // used by the CLIENTcontroller to create the message to be sent to the server
    QJsonObject toJson() const;
    static Char fromJson(const QJsonObject& _JSONobj);
};


#endif //POLIDOX_CHAR_H
