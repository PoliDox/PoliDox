#include "Char.h"


Char::~Char()
{
}

void Char::setPosition(std::vector<int> x)
{
    this->position = x;
}

QJsonDocument Char::write(const QString &action){

  QJsonObject _JSONobj;
  QJsonArray _JSONpos;

  _JSONobj.insert("action",action);
  _JSONobj.insert("value",this->value);
  _JSONobj.insert("siteId",this->siteId);
  _JSONobj.insert("counter",this->counter);

  for(auto it=this->position.begin();it!=this->position.end();it++){
      _JSONpos.push_back(*it);
  }

  _JSONobj.insert("position",_JSONpos);

  QJsonDocument _JSONdoc(_JSONobj);

  return _JSONdoc;
}

Char Char::read(const QString& _JSONstring){

    QJsonDocument _JSONdoc=QJsonDocument::fromJson(_JSONstring.toUtf8());

    if(!_JSONdoc.isNull()){

        QJsonObject _JSONobj=_JSONdoc.object();
        QString value=_JSONobj["value"].toString();
        int counter=_JSONobj["counter"].toInt();
        int siteId=_JSONobj["siteId"].toInt();
        std::vector<int> position;

        QJsonArray _JSONpos=_JSONobj["position"].toArray();

        std::cout << "Remote insert of symbol "<< value[0].toLatin1() <<" at position [ ";
        for(QJsonArray::iterator it=_JSONpos.begin();it!=_JSONpos.end();it++){
            position.push_back(it->toInt());
            std::cout<< it->toInt() <<" ";
        }

        std::cout << "]\n\n" << std::endl;
        Char symbol(siteId,counter,value[0].toLatin1());
        symbol.setFractionalPosition(position);

        return symbol;
    }




}

vector<int> Char::getPosition() {
    return position;
}

char Char::getValue() {
    return value;
}

void Char::setFractionalPosition(vector<int> fractionalPosition) {
        this->position = fractionalPosition;
}

vector<int> Char::getFractionalPosition() const {
    return position;
}
