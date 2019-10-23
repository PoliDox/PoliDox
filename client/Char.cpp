#include "Char.h"



Char::Char(double siteId, int counter, char value) : siteId(siteId), counter(counter), value(value){
    //nothing else to do??
}


Char::~Char(){
    //TODO
}


double Char::getSiteId() const {
    return siteId;
}

void Char::setPosition(std::vector<int>& x)
{
    this->position = x;
}

QJsonObject Char::toJson() const {

    QJsonObject _JSONobj;
    QJsonArray _JSONpos;

    _JSONobj.insert("value",QString(this->value));
    _JSONobj.insert("siteId",this->siteId);
    _JSONobj.insert("counter",this->counter);

    for(auto it=this->position.begin();it!=this->position.end();it++){
        _JSONpos.push_back(*it);
    }

    _JSONobj.insert("position",_JSONpos);

    return _JSONobj;
}


//TODO: dove viene usata questa funzione?
Char Char::fromJson(const QJsonObject& _JSONobj){
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
    Char symbol(siteId,counter,value[0].toLatin1());
    symbol.setFractionalPosition(position);

    return symbol;
}


Char Char::fromJson2(const QString& stringReturnedFromDb){
    QJsonObject stringObjJSON;
    QJsonDocument stringDocJSON;

    stringDocJSON = QJsonDocument::fromJson(stringReturnedFromDb.toUtf8());
    if (stringDocJSON.isNull()) {
        // TODO: print some debug
        return Char(-1,-1,'z');     //TODO: da sistemare, questa riga va cancellata, bisogna lanciare l'eccezione
    }
    stringObjJSON = stringDocJSON.object();

    QString value = stringObjJSON["value"].toString();
    int counter = stringObjJSON["counter"].toInt();
    double siteId = stringObjJSON["siteId"].toDouble();
    QJsonArray fractionalPositionObjJSON = stringObjJSON["position"].toArray();

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON)
        fractionalPosition.push_back(elem.toInt());

    Char result(siteId, counter, value.at(0).toLatin1())    ;  //TODO: sistemare siteid e counter? perche sono in char? non dovrebbero esserci
    result.setFractionalPosition(fractionalPosition);
    return result;
}



// Used in DatabaseManager::getAllInserts where
// we take the documents of the collection "insert",
// that are strings formatted in Json inside
Char Char::fromJson(const QString& stringReturnedFromDb){
    QJsonObject stringObjJSON;
    QJsonDocument stringDocJSON;

    stringDocJSON = QJsonDocument::fromJson(stringReturnedFromDb.toUtf8());
    if (stringDocJSON.isNull()) {
        // TODO: print some debug
        return Char(-1,-1,'z');     //TODO: da sistemare, questa riga va cancellata, bisogna lanciare l'eccezione
    }
    stringObjJSON = stringDocJSON.object();

    QString symbol = stringObjJSON["symbol"].toString();
    QJsonArray fractionalPositionObjJSON = stringObjJSON["fractionalPosition"].toArray();

    std::vector<int> fractionalPosition;
    for(auto elem : fractionalPositionObjJSON)
        fractionalPosition.push_back(elem.toInt());

    Char result(-1, -1, symbol.at(0).toLatin1());  //TODO: sistemare siteid e counter? perche sono in char? non dovrebbero esserci
    result.setFractionalPosition(fractionalPosition);
    return result;
}


std::vector<int> Char::getPosition() {
    return this->position;
}


char Char::getValue() const {
    return this->value;
}


void Char::setFractionalPosition(std::vector<int>& fractionalPosition) {
        this->position = fractionalPosition;
}


std::vector<int> Char::getFractionalPosition() {
    return position;
}


bool Char::operator < (const Char& other) const {
    return this->position < other.position;
}
