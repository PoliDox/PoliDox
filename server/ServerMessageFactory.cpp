#include "ServerMessageFactory.h"


//QByteArray jsonString = ClientMessageFactory::createInsertMessage(symbol);
//m_socket.sendTextMessage(jsonString);

QByteArray ServerMessageFactory::createNewClientMessage(const Account& account) {
    QJsonObject l_obj;
    l_obj.insert("action", "newClient");    
    QJsonValue jsonAccount(account.toJson());
    l_obj.insert("account", jsonAccount);

    QJsonDocument l_doc(l_obj);
    return l_doc.toJson(QJsonDocument::Indented);
}


// if "response" == false then "account" must be null
QByteArray ServerMessageFactory::createLoginReply(bool response, const Account *account, QList<QString>& nameDocuments) {
    QJsonObject objToReturn;
    QJsonArray nameDocumentsArray;
    QString responseString;
    if(response)
        responseString = "ok";
    else
        responseString = "fail";

    objToReturn.insert("action","loginRepl");
    objToReturn.insert("response",responseString);
    if(account != nullptr)
        objToReturn.insert("account", account->toJson());
    else
        objToReturn.insert("account", "null");
    for(auto nameDocument : nameDocuments){
        nameDocumentsArray.push_back(nameDocument);
    }
    objToReturn.insert("nameDocuments",nameDocumentsArray);

    QJsonDocument docOfObj(objToReturn);
    return docOfObj.toJson(QJsonDocument::Indented);
}


QByteArray ServerMessageFactory::createRegistrationUserReply(double siteIdOfRegisteredUser){
    QJsonObject objToReturn;
    QString responseString;

    objToReturn.insert("action","regUsrRepl");
    objToReturn.insert("siteId",siteIdOfRegisteredUser);

    QJsonDocument docOfObj(objToReturn);
    return docOfObj.toJson(QJsonDocument::Indented);
}
