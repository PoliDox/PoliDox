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
    ushort value;
    std::vector<int> fractionalPosition;
    int siteId;  // DO NOT DELETE: it is used to move the cursors in remote operations
    tStyle style;

public:
    Char(ushort p_value, int p_siteId, std::vector<int>& p_fractionalPosition);
    Char(ushort p_value, int p_siteId);
    Char() = default;
    std::vector<int> getFractionalPosition();
    void setFractionalPosition(std::vector<int>& fractionalPosition);
    ushort getValue() const;
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
