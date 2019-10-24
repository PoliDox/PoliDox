#include "Char.h"



Char::Char(char value, std::vector<int>& fractionalPosition){
    this->value = value;
    this->fractionalPosition = fractionalPosition;
}


Char::Char(char value){
    this->value = value;
}


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


bool Char::operator < (const Char& other) const {
    return this->fractionalPosition < other.fractionalPosition;
}


QJsonObject Char::toJson() const {
    QJsonObject charJSON;
    QJsonArray fractPosJSON;

    QString charValue(this->value);
    charJSON.insert("value", charValue);
    for(int elem : this->fractionalPosition)
        fractPosJSON.push_back(elem);
    charJSON.insert("position",fractPosJSON);       //TODO: cambiare in fractionalPosition

    return charJSON;
}


Char Char::fromJson(const QJsonObject& charJSON){
    // TODO: What if some value is missing? E.g. There is no "value"
    char value = charJSON["value"].toString().at(0).toLatin1();
    QJsonArray fractionalPositionObjJSON = charJSON["position"].toArray();  //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON){
        fractionalPosition.push_back(elem.toInt());
    }

    Char result(value, fractionalPosition);
    return result;
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





