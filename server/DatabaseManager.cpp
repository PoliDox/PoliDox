#include "DatabaseManager.h"



void DatabaseManager::incrementCounterOfCollection(QString nameCollection){
    mongocxx::collection countersCollection = (*this->db)["counter"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value collToincrement =
       elementBuilder << "_id" << nameCollection.toUtf8().constData()
                      << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view collToincrementView = collToincrement.view();

    auto elementBuilder2 = bsoncxx::builder::stream::document{};
    bsoncxx::document::value incrementSequence =
       elementBuilder2 << "$inc" << bsoncxx::builder::stream::open_document
                                 << "sequentialCounter" << 1
                                 << bsoncxx::builder::stream::close_document
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view incrementSequenceView = incrementSequence.view();

    countersCollection.update_one(collToincrementView, incrementSequenceView);
}


void DatabaseManager::insertNewElemInCounterCollection(QString nameDocument, int initialValue){
    mongocxx::collection countersCollection = (*this->db)["counter"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value elemToInsert =
        elementBuilder << "_id"                 << nameDocument.toUtf8().constData()
                       << "sequentialCounter"   << initialValue
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view elemToInsertView = elemToInsert.view();

    try {
        countersCollection.insert_one(elemToInsertView);
    } catch (std::exception e) {
        //caso elemento con "_id" già presente
        //TODO: gestire l'eccezione. ritornare un valore di errore ?
    }

}


int DatabaseManager::getCounterOfCollection(QString nameCollection){
    mongocxx::collection countersCollection = (*this->db)["counter"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value collToRetrieve =
        elementBuilder << "_id" << nameCollection.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view collToRetrieveView = collToRetrieve.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = countersCollection.find_one(collToRetrieveView);
    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();
        bsoncxx::document::element element = a["sequentialCounter"];
        return element.get_int32().value;
    }
    else{
        //it's the first time; so insert new element in counter collection
        //with fields: "_id":nameCollection , "sequentialCounter":firstValue
        int initialValue = 0;
        insertNewElemInCounterCollection(nameCollection, initialValue);
        return initialValue;
    }
}


//TODO: - inserire fra i parametri nome utente e psw con cui accedere al db
//      - gestire eccezione in caso di mancata connessione
//      - implementare come singleton
//Alla creazione dell'istanza viene creata la connessione con il db
DatabaseManager::DatabaseManager() {
    this->instance = new mongocxx::instance {};                         //this should be done only once.
    this->uri = new mongocxx::uri("mongodb://localhost:27017");
    this->client =  new mongocxx::client( *(this->uri) );               //client istance connected to MongoDB server
    this->db = new mongocxx::database( (*this->client)["polidox"] );    //connection to "polidox" db of MongoDB
}


//TODO: - sistemare il blocco catch
//      - inserire l'immagine
// - Returns siteId if the registration success, -1 otherwise
int DatabaseManager::registerUser(QString& name, QString& password, QByteArray& image){
    mongocxx::collection userCollection = (*this->db)["user"];

    int siteId = this->getCounterOfCollection("user");

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

/*  serve trasformala ad unsigned char?? per ora forse no */
/*    std::vector<unsigned char> imageVector(image.begin(), image.end());
    auto array_builder = bsoncxx::builder::basic::array{};
    for (unsigned char element : imageVector) {
        array_builder.append(element);
    } */
    auto array_builder = bsoncxx::builder::basic::array{};
    for(signed char elem : image){
        array_builder.append(elem);
    }

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userToInsert =
        elementBuilder << "_id"      << name.toUtf8().constData()
                       << "password" << hashedPsw.toUtf8().constData()
                       << "siteId"   << siteId
                       << "image"    << array_builder
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view userToInsertView = userToInsert.view();

    try {
        userCollection.insert_one(userToInsertView);
    } catch (std::exception e) {
        //caso utente con "name" già presente
        return -1;
    }

    this->incrementCounterOfCollection("user");
    return siteId;
}


QByteArray DatabaseManager::getImage(bsoncxx::document::view queryResult){
    QString userDocument = QString::fromStdString(bsoncxx::to_json(queryResult));
    QJsonDocument stringDocJSON = QJsonDocument::fromJson(userDocument.toUtf8());
    if (stringDocJSON.isNull()) {
        // TODO: print some debug
        throw "DatabaseManager::getImage  error";       //TODO: da sistemare
    }
    QJsonObject userObjJson = stringDocJSON.object();

    QJsonArray imageObjJSON = userObjJson["image"].toArray();
    QByteArray imageToReturn;
    for(auto elem : imageObjJSON){
        char appo = (char)elem.toInt();     //TODO: da rivedere!! se funziona in tutti i casi, potrebbero verificarsi casi in cui taglia qualcosa e perdo informazione?
        imageToReturn.push_back(appo);
    }

    return imageToReturn;
}


//it returns the id of the account, -1 otherwise
int DatabaseManager::checkPassword(QString& name, QString& password, QByteArray& imageToReturn){
    mongocxx::collection userCollection = (*this->db)["user"];

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userToCheck =
        elementBuilder  << "_id"      << name.toUtf8().constData()
                        << "password" << hashedPsw.toUtf8().constData()
                        << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view userToCheckView = userToCheck.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = userCollection.find_one(userToCheckView);
    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();

        bsoncxx::document::element element = a["siteId"];
        int siteIdToReturn = element.get_int32().value;

        imageToReturn = getImage(a);

        return siteIdToReturn;
    }
    else{
        return -1;
    }
}


//TODO: - sistemare bene l'eccezione
// documentName is unique
bool DatabaseManager::insertNewDocument(QString& documentName, QString& uri){
    mongocxx::collection documentCollection = (*this->db)["document"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value docToInsert =
        elementBuilder << "_id" << documentName.toUtf8().constData()
                       << "uri" << uri.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view docToInsertView = docToInsert.view();

    try {
        documentCollection.insert_one(docToInsertView);
    } catch (std::exception e) {
        //fail, there is alredy a document with that name
        return false;
    }
    return true;
}


bool DatabaseManager::insertNewPermission(QString& nameDocument, int siteId){
    mongocxx::collection documentPermCollection = (*this->db)["documentPermission"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value permToInsert =
        elementBuilder << "_id" << bsoncxx::builder::stream::open_document
                            << "nameDocument" << nameDocument.toUtf8().constData()
                            << "siteId"       << siteId
                            << bsoncxx::builder::stream::close_document
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view permToInsertView = permToInsert.view();

    try {
        documentPermCollection.insert_one(permToInsertView);
    } catch (std::exception e) {
        // in case of fail, it means that the user still has the permission
        // for that  document, so we can simply ignore the exception
        return false;
    }
    return true;
}


QString DatabaseManager::getUri(QString& documentName){
    mongocxx::collection documentCollection = (*this->db)["document"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value documentToGet =
        elementBuilder  << "_id" << documentName.toUtf8().constData()
                        << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view documentToGetView = documentToGet.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = documentCollection.find_one(documentToGetView);
    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();
        bsoncxx::document::element uri = a["uri"];
        bsoncxx::stdx::string_view uriView = uri.get_utf8().value;
        QString appo = QString::fromStdString( uriView.to_string() );
        return appo;
    }
    else {
        throw "DatabaseManager::getUri exception";
    }
}


// return an empty string if the uri passed here does not have
// a document associated
QString DatabaseManager::getDocument(QString& uriOfDocument){
    mongocxx::collection documentCollection = (*this->db)["document"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value documentToGet =
        elementBuilder  << "uri" << uriOfDocument.toUtf8().constData()
                        << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view documentToGetView = documentToGet.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = documentCollection.find_one(documentToGetView);
    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();
        bsoncxx::document::element nameDocument = a["_id"];
        bsoncxx::stdx::string_view nameDocumentView = nameDocument.get_utf8().value;
        QString appo = QString::fromStdString( nameDocumentView.to_string() );
        return appo;
    }
    else {
        return QString("");
    }
}



//TODO: - sistemare il valore di ritorno
//      - implementare il vincolo di integrità su nameDocument
//      - anziché passargli tutti questi attributi, passargli un char e chiamare poi qui dentro il toJson
bool DatabaseManager::insertSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition,
                                   QString& family, int size, bool bold, bool italic, bool underline, int alignment) {
    mongocxx::collection insertCollection = (*this->db)["insert"];

    int counterInsert = this->getCounterOfCollection("insert");

    auto array_builder = bsoncxx::builder::basic::array{};
    for (int element : fractionalPosition) {
        array_builder.append(element);
    }

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value symbolToInsert =
        elementBuilder << "_id"                << counterInsert
                       << "nameDocument"       << nameDocument.toUtf8().constData()
                       << "value"              << symbol.toUtf8().constData()
                       << "siteId"             << siteIdOfSymbol
                       << "position"           << array_builder
                       << "style"              << bsoncxx::builder::stream::open_document
                            << "fontFamily" << family.toUtf8().constData()
                            << "fontSize"   << size
                            << "bold"       << bold
                            << "italic"     << italic
                            << "underline"  << underline
                            << "alignment"  << (int)0       // TODO: ALIGNMENT
                            << bsoncxx::builder::stream::close_document
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view symbolToInsertView = symbolToInsert.view();

    try {
        insertCollection.insert_one(symbolToInsertView);
    } catch (std::exception e) {
        //in quale caso potrebbe fallire?
        return false;
    }

    this->incrementCounterOfCollection("insert");
    return true;
}


//TODO: - sistemare il valore di ritorno, gestire
//        eventuale eccezione alzata dalla delete_one
//A symbol is uniquely identified by his fractional position and belonging document,
//so "symbol parameter" is useful, we pass it only for sake of completeness
bool DatabaseManager::deleteSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition){
    mongocxx::collection insertCollection = (*this->db)["insert"];

    auto array_builder = bsoncxx::builder::basic::array{};
    for (int element : fractionalPosition) {
        array_builder.append(element);
    }

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value symbolToDelete =
        elementBuilder << "nameDocument"       << nameDocument.toUtf8().constData()
                       << "value"              << symbol.toUtf8().constData()
                       << "siteId"             << siteIdOfSymbol
                       << "position"           << array_builder
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view symbolToDeleteView = symbolToDelete.view();

    try {
        insertCollection.delete_one(symbolToDeleteView);
    } catch (std::exception e) {
        //in quale caso potrebbe fallire?
        return false;
    }
    return true;
}


QList<Char> DatabaseManager::getAllInserts(QString& nameDocument){
    QList<Char> orderedChars;
    mongocxx::collection insertCollection = (*this->db)["insert"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value insertsToRetrieve =
        elementBuilder << "nameDocument" << nameDocument.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view insertsToRetrieveView = insertsToRetrieve.view();

    mongocxx::cursor resultIterator = insertCollection.find(insertsToRetrieveView);

    //(.1)Save them in local before ordering
    for (auto elem : resultIterator) {
        QString insert = QString::fromStdString(bsoncxx::to_json(elem));
        QJsonDocument stringDocJSON = QJsonDocument::fromJson(insert.toUtf8());
        if (stringDocJSON.isNull()) {
            // TODO: print some debug
            throw "DatabaseManager::getAllInserts  error";       //TODO: da sistemare
        }
        QJsonObject insertObjJson = stringDocJSON.object();

        Char charToInsert = Char::fromJson(insertObjJson);

        orderedChars.push_back(charToInsert);
    }

    //(.2)Now ordering.
    //It orders according to the order established in Char object,
    //so will be returned a vector in fractionalPosition ascending order
    std::sort(orderedChars.begin(), orderedChars.end());
    return orderedChars;
}


QList<int> DatabaseManager::getAllAccounts(QString& nameDocument){
    QList<int> siteIdAccounts;
    mongocxx::collection documentCollection = (*this->db)["documentPermission"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value accountsToRetrieve =
        elementBuilder << "_id.nameDocument" << nameDocument.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view accountsToRetrieveView = accountsToRetrieve.view();

    mongocxx::cursor resultIterator = documentCollection.find(accountsToRetrieveView);
    for(auto elem : resultIterator) {
        bsoncxx::document::element siteIdAccount = elem["_id"]["siteId"];
        int siteId = siteIdAccount.get_int32().value;
        siteIdAccounts.append(siteId);
    }

    return siteIdAccounts;
}


QList<Account> DatabaseManager::getAllAccounts(QList<int>& siteIdAccounts){
    QList<Account> accounts;

    for(int siteId : siteIdAccounts){
        accounts.append( this->getAccount(siteId) );
    }

    return accounts;
}



Account DatabaseManager::getAccount(int siteId){
    mongocxx::collection userCollection = (*this->db)["user"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value accountToRetrieve =
        elementBuilder << "siteId" << siteId
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view accountToRetrieveView = accountToRetrieve.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = userCollection.find_one(accountToRetrieveView);
    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();

        QString accountString = QString::fromStdString(bsoncxx::to_json(a));
        QJsonDocument stringDocJSON = QJsonDocument::fromJson(accountString.toUtf8());
        if (stringDocJSON.isNull()) {
            // TODO: print some debug
            throw "DatabaseManager::getAccount(int)  error";
        }

        QJsonObject accountObjJson = stringDocJSON.object();
        Account accountToReturn = Account::fromJson(accountObjJson, true);
        return accountToReturn;
    }
    else {
        throw "DatabaseManager::getAccount(int) query result error";       //TODO: da sistemare
    }
}


QList<QString> DatabaseManager::getAllDocuments(int siteId){
    QList<QString> nameDocuments;
    mongocxx::collection documentCollection = (*this->db)["documentPermission"];
    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value documentsToRetrieve =
        elementBuilder << "_id.siteId" << siteId
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view documentsToRetrieveView = documentsToRetrieve.view();

    mongocxx::cursor resultIterator = documentCollection.find(documentsToRetrieveView);
    for(auto elem : resultIterator) {
        bsoncxx::document::element nameDocument = elem["_id"]["nameDocument"];
        bsoncxx::stdx::string_view nameDocumentView = nameDocument.get_utf8().value;
        QString appo = QString::fromStdString( nameDocumentView.to_string() );

        nameDocuments.append(appo);
    }

    return nameDocuments;
}


mongocxx::database DatabaseManager::getDb(){
    return *(this->db);
}


DatabaseManager::~DatabaseManager() {
    delete (this->uri);
    delete (this->client);
    delete (this->db);
    delete (this->instance);
}

