#include "Char.h"
#include <iostream>


Char::Char(char p_value, int p_siteId, std::vector<int>& p_fractionalPosition)
    : value(p_value), siteId(p_siteId), fractionalPosition(p_fractionalPosition)
{}


Char::Char(char p_value, int p_siteId) : value(p_value), siteId(p_siteId)
{}


Char::~Char(){
    //TODO
}


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
    for(int elem : this->fractionalPosition)
        fractPosJSON.push_back(elem);
    charJSON.insert("position",fractPosJSON);       //TODO: cambiare in fractionalPosition

    return charJSON;
}


Char Char::fromJson(const QJsonObject& charJSON){
    // TODO: What if some value is missing? E.g. There is no "value"
    char value = charJSON["value"].toString().at(0).toLatin1();
    int siteId = (int)charJSON["siteId"].toDouble();
    QJsonArray fractionalPositionObjJSON = charJSON["position"].toArray();  //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON){
        fractionalPosition.push_back(elem.toInt());
    }

    Char result(value, siteId, fractionalPosition);
    return result;
}

void Char::setStyle(QString family, int size, int bold, int italic, int underline,int al){

    st.font_family=family;
    st.font_size=size;
    st.is_bold=bold;
    st.is_italic=italic;
    st.is_underline=underline;
    st.alignment=al;
}

style Char::getStyle(){

    return st;
}

//TODO: delete this???
/*
Char Char::fromJson2(const QString& stringReturnedFromDb){
    QJsonObject stringObjJSON;
    QJsonDocument stringDocJSON;

    stringDocJSON = QJsonDocument::fromJson(stringReturnedFromDb.toUtf8());
    if (stringDocJSON.isNull()) {
        // TODO: print some debug
        throw "Char::fromJson2 error: malformatted Json string";     //TODO: da sistemare, questa riga va cancellata, bisogna lanciare l'eccezione
    }
    stringObjJSON = stringDocJSON.object();

    //int counter = stringObjJSON["counter"].toInt();
    //double siteId = stringObjJSON["siteId"].toDouble();
    QString value = stringObjJSON["value"].toString();              //TODO: mettersi d'accordo sul campo, o symbol o value
    QJsonArray fractionalPositionObjJSON = stringObjJSON["position"].toArray(); //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON)
        fractionalPosition.push_back(elem.toInt());

    char valueOfChar = value.at(0).toLatin1();
    Char result(valueOfChar, fractionalPosition)    ;  //TODO: sistemare siteid e counter? perche sono in char? non dovrebbero esserci
    return result;
}
*/





