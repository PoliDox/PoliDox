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

    charJSON.insert("value", QString(this->value));
    for(int elem : this->fractionalPosition)
        fractPosJSON.push_back(elem);
    charJSON.insert("position",fractPosJSON);       //TODO: cambiare in fractionalPosition

    return charJSON;
}


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


// Used in DatabaseManager::getAllInserts where
// we take the documents of the collection "insert",
// that are strings formatted in Json inside
//TODO: QUESTO METODO PUO' RICHIAMARE QUELLO AVENTE COME PARAMETRO IL QJsonObject
//      MA PRIMA METTERDI D'ACCORDO SUI CAMPI FRA symbol E position
Char Char::fromJson(const QString& stringReturnedFromDb){
    QJsonObject stringObjJSON;
    QJsonDocument stringDocJSON;

    stringDocJSON = QJsonDocument::fromJson(stringReturnedFromDb.toUtf8());
    if (stringDocJSON.isNull()) {
        // TODO: print some debug
        throw "Char::fromJson error: malformatted Json string";     //TODO: da sistemare, questa riga va cancellata, bisogna lanciare l'eccezione
    }
    stringObjJSON = stringDocJSON.object();

    QString symbol = stringObjJSON["symbol"].toString();            //TODO: metterdi d'accordo sul campo, o symbol o value
    QJsonArray fractionalPositionObjJSON = stringObjJSON["fractionalPosition"].toArray(); //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON)
        fractionalPosition.push_back(elem.toInt());

    char valueOfChar = symbol.at(0).toLatin1();
    Char result(valueOfChar, fractionalPosition)    ;  //TODO: sistemare siteid e counter? perche sono in char? non dovrebbero esserci
    return result;
}


Char Char::fromJson(const QJsonObject& charJSON){
    // TODO: What if some value is missing? E.g. There is no "value"
    QString value = charJSON["value"].toString();
    //int counter=charJSON["counter"].toInt();
    //int siteId=charJSON["siteId"].toInt();
    QJsonArray fractionalPositionObjJSON = charJSON["position"].toArray();  //TODO: mettersi d'accordo sul campo, position o fractionalPosition

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON){
        fractionalPosition.push_back(elem.toInt());
    }

    char valueOfChar = value.at(0).toLatin1();
    Char result(valueOfChar, fractionalPosition)    ;  //TODO: sistemare siteid e counter? perche sono in char? non dovrebbero esserci
    return result;
}


//TODO: dove viene usata questa funzione?
/*Char Char::fromJson(const QJsonObject& _JSONobj){
    // TODO: What if some value is missing? E.g. There is no "value"
    QString value=_JSONobj["value"].toString();
    int counter=_JSONobj["counter"].toInt();
    int siteId=_JSONobj["siteId"].toInt();
    std::vector<int> position;

    QJsonArray _JSONpos=_JSONobj["position"].toArray();

    //std::cout << "Remote "<< action.toUtf8().constData() << " of symbol "<< value.toUtf8().constData() <<" at position [ ";
    for(QJsonArray::iterator it=_JSONpos.begin();it!=_JSONpos.end();it++){
        position.push_back(it->toInt());
        //std::cout<< it->toInt() <<" ";
    }

    //std::cout << "]" << std::endl;
    //Char symbol(siteId,counter,value[0].toLatin1());
    symbol.setFractionalPosition(position);

    return symbol;
}*/








