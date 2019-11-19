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

typedef struct Style {
    // DON'T CHANGE THE ORDER without changing fromJson()
    QString font_family;
    int font_size;
    bool is_bold;
    bool is_italic;
    bool is_underline;
    int alignment;

} tStyle;

class Char {

private:
    char value;
    std::vector<int> fractionalPosition;
    int siteId;  // NON CANCELLARE: serve per spostare i cursori nelle operazioni remote
    tStyle style;

public:
    Char(char p_value, int p_siteId, std::vector<int>& p_fractionalPosition);
    Char(char p_value, int p_siteId);
    Char() = default;
    std::vector<int> getFractionalPosition();
    void setFractionalPosition(std::vector<int>& fractionalPosition);
    char getValue() const;
    bool operator < (const Char& other) const;
    ~Char();
    int getSiteId();

    // used by the CLIENTcontroller to create the message to be sent to the server
    QJsonObject toJson() const;
    static Char fromJson(const QJsonObject& _JSONobj);    

    void setStyle(QString family, int size, bool bold, bool italic, bool underline, int aligmnent);
    tStyle getStyle() const { return style; }

};


#endif //POLIDOX_CHAR_H
