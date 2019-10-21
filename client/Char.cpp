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

void Char::setPosition(std::vector<int> x)
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

vector<int> Char::getPosition() {
    return position;
}

char Char::getValue() const {
    return value;
}

void Char::setFractionalPosition(vector<int> fractionalPosition) {
        this->position = fractionalPosition;
}

vector<int> Char::getFractionalPosition() const {
    return position;
}
