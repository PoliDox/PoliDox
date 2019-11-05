#include "Char.h"
#include <iostream>


Char::Char(char p_value, int p_siteId, std::vector<int>& p_fractionalPosition)
    : value(p_value), siteId(p_siteId), fractionalPosition(p_fractionalPosition)
{}


Char::Char(char p_value, int p_siteId) : value(p_value), siteId(p_siteId)
{}


Char::~Char(){ }


void Char::setFractionalPosition(std::vector<int>& newFractPos) {
    this->fractionalPosition = newFractPos;
}


std::vector<int> Char::getFractionalPosition() {
    return this->fractionalPosition;
}


char Char::getValue() const {
    return this->value;
}


int Char::getSiteId(){
    return siteId;
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

    // TODO: What about a tStyle::toJson function?
    QJsonObject styleJSON;
    styleJSON.insert("fontFamily", style.font_family);
    styleJSON.insert("fontSize", style.font_size);
    styleJSON.insert("bold", style.is_bold);
    styleJSON.insert("italic", style.is_italic);
    styleJSON.insert("underline", style.is_underline);
    // TODO: ALIGNMENT
    styleJSON.insert("alignment", 0);
    charJSON.insert("style", styleJSON);

    return charJSON;
}


Char Char::fromJson(const QJsonObject& charJSON){
    char value = charJSON["value"].toString().at(0).toLatin1();
    int siteId = (int)charJSON["siteId"].toDouble();
    QJsonArray fractionalPositionObjJSON = charJSON["position"].toArray();  //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON){
        fractionalPosition.push_back(elem.toInt());
    }

    QJsonObject styleJSON = charJSON["style"].toObject();
    tStyle l_style = { styleJSON["fontFamily"].toString(), styleJSON["fontSize"].toInt(),
                       styleJSON["bold"].toInt(), styleJSON["italic"].toInt(),
                       styleJSON["underline"].toInt() };
    // TODO: ALIGNMENT!!

    Char result(value, siteId, fractionalPosition);
    result.style = l_style;

    return result;
}


void Char::setStyle(QString family, int size, int bold, int italic, int underline,int al){

    style.font_family=family;
    style.font_size=size;
    style.is_bold=bold;
    style.is_italic=italic;
    style.is_underline=underline;
    style.alignment=al;
}

tStyle Char::getStyle(){

    return style;
}

