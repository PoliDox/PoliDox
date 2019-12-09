#include "Char.h"
#include <iostream>


Char::Char(ushort p_value, int p_siteId, std::vector<int>& p_fractionalPosition)
    : value(p_value), fractionalPosition(p_fractionalPosition), siteId(p_siteId)
{}


Char::Char(ushort p_value, int p_siteId) : value(p_value), siteId(p_siteId)
{}


Char::~Char(){ }


void Char::setFractionalPosition(std::vector<int>& newFractPos) {
    this->fractionalPosition = newFractPos;
}


std::vector<int> Char::getFractionalPosition() {
    return this->fractionalPosition;
}


ushort Char::getValue() const {
    return this->value;
}


int Char::getSiteId(){
    return this->siteId;
}


bool Char::operator < (const Char& other) const {
    return this->fractionalPosition < other.fractionalPosition;
}


QJsonObject Char::toJson() const {
    QJsonObject charJSON;
    QJsonArray fractPosJSON;

    QString charValue(this->value);
    charJSON.insert("value", charValue);
    charJSON.insert("siteId", siteId);
    for(int elem : this->fractionalPosition) {
        fractPosJSON.push_back(elem);
    }
    charJSON.insert("position",fractPosJSON);

    QJsonObject styleJSON;
    styleJSON.insert("fontFamily", style.font_family);
    styleJSON.insert("fontSize", style.font_size);
    styleJSON.insert("bold", style.is_bold);
    styleJSON.insert("italic", style.is_italic);
    styleJSON.insert("underline", style.is_underline);    
    styleJSON.insert("alignment", style.alignment);
    charJSON.insert("style", styleJSON);

    return charJSON;
}


Char Char::fromJson(const QJsonObject& charJSON){
    ushort value = charJSON["value"].toString().at(0).unicode();
    int siteId = charJSON["siteId"].toInt();
    QJsonArray fractionalPositionObjJSON = charJSON["position"].toArray();

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON){
        fractionalPosition.push_back(elem.toInt());
    }

    QJsonObject styleJSON = charJSON["style"].toObject();
    tStyle l_style = { styleJSON["fontFamily"].toString(), styleJSON["fontSize"].toInt(),
                       styleJSON["bold"].toBool(), styleJSON["italic"].toBool(),
                       styleJSON["underline"].toBool(), styleJSON["alignment"].toInt() };


    Char result(value, siteId, fractionalPosition);
    result.style = l_style;

    return result;
}


void Char::setStyle(QString family, int size, bool bold, bool italic, bool underline, int al){

    style.font_family = family;
    style.font_size = size;
    style.is_bold = bold;
    style.is_italic = italic;
    style.is_underline = underline;
    style.alignment = al;
}

std::ostream & operator << (std::ostream &out, const Char &c)
{
    out << c.getValue();
    return out;
}
