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

typedef struct Style{

    QString font_family;
    int font_size;
    int is_bold;
    int is_italic;
    int is_underline;
    Qt::Alignment alignment;

}style;

class Char {

private:
    char value;
    std::vector<int> fractionalPosition;
    int siteId;  // NON CANCELLARE: serve per spostare i cursori nelle operazioni remote
    style st;

public:
    Char(char p_value, int p_siteId, std::vector<int>& p_fractionalPosition);
    Char(char p_value, int p_siteId);
    std::vector<int> getFractionalPosition();
    void setFractionalPosition(std::vector<int>& fractionalPosition);
    char getValue() const;
    bool operator < (const Char& other) const;
    ~Char();
    int getSiteId();

    // used by the CLIENTcontroller to create the message to be sent to the server
    QJsonObject toJson() const;
    static Char fromJson(const QJsonObject& _JSONobj);
    //static Char fromJson2(const QString& stringReturnedFromDb);

    void setStyle(QString family, int size, int bold, int italic, int underline,Qt::Alignment al);
    style getStyle();
};


#endif //POLIDOX_CHAR_H
