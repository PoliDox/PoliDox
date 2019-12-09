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

    try{
        countersCollection.update_one(collToincrementView, incrementSequenceView);
    } catch (mongocxx::query_exception e){
        qDebug() << "[ERROR][DatabaseManager::incrementCounterOfCollection] update_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }
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
    } catch (mongocxx::operation_exception e) {
        qDebug() << "[ERROR][DatabaseManager::insertNewElemInCounterCollection] insert_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }

}


int DatabaseManager::getCounterOfCollection(QString nameCollection){
    mongocxx::collection countersCollection = (*this->db)["counter"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value collToRetrieve =
        elementBuilder << "_id" << nameCollection.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view collToRetrieveView = collToRetrieve.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult;
    try{
        queryResult= countersCollection.find_one(collToRetrieveView);
    } catch (mongocxx::query_exception e){
        qDebug() << "[ERROR][DatabaseManager::getCounterOfCollection] find_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }

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


DatabaseManager::DatabaseManager() {
    this->instance = new mongocxx::instance {};                         //this should be done only once.
    this->uri = new mongocxx::uri("mongodb://localhost:27017");
    this->client =  new mongocxx::client( *(this->uri) );               //client istance connected to MongoDB server
    this->db = new mongocxx::database( (*this->client)["polidox"] );    //connection to "polidox" db of MongoDB
}


// - Returns siteId if the registration success, -1 otherwise
int DatabaseManager::registerUser(QString& name, QString& password, QByteArray& image){
    mongocxx::collection userCollection = (*this->db)["user"];

    int siteId = this->getCounterOfCollection("user");

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

    std::vector<unsigned char> imageVector(image.begin(), image.end());
    bsoncxx::types::b_binary img {bsoncxx::binary_sub_type::k_binary,
                                  uint32_t(imageVector.size()),
                                  imageVector.data()};

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userToInsert =
        elementBuilder << "_id"      << name.toUtf8().constData()
                       << "password" << hashedPsw.toUtf8().constData()
                       << "siteId"   << siteId
                       << "image"    << img
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view userToInsertView = userToInsert.view();

    try {
        userCollection.insert_one(userToInsertView);
    } catch (mongocxx::bulk_write_exception e) {
        return -1;
    }
    catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::registerUser] insert_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }

    this->incrementCounterOfCollection("user");
    return siteId;
}


bool DatabaseManager::changeImage(QString& nameAccount, QByteArray& newImage){
    mongocxx::collection userCollection = (*this->db)["user"];

    std::vector<unsigned char> imageVector(newImage.begin(), newImage.end());
    bsoncxx::types::b_binary img {bsoncxx::binary_sub_type::k_binary,
                                  uint32_t(imageVector.size()),
                                  imageVector.data()};

    try {
        userCollection.update_one(bsoncxx::builder::stream::document{}
                                    << "_id" << nameAccount.toUtf8().constData()
                                    << bsoncxx::builder::stream::finalize,
                                  bsoncxx::builder::stream::document{}
                                    << "$set" << bsoncxx::builder::stream::open_document
                                    << "image" << img
                                    << bsoncxx::builder::stream::close_document
                                    << bsoncxx::builder::stream::finalize);
    } catch (mongocxx::bulk_write_exception e){
        //image update went wrong, due to an excessive size of the image
        return false;
    }
    catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::changeImage] update_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }

    return true;
}


bool DatabaseManager::changePassword(QString& nameAccount, QString& password){
    mongocxx::collection userCollection = (*this->db)["user"];

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

    try{
        userCollection.update_one(bsoncxx::builder::stream::document{}
                                    << "_id" << nameAccount.toUtf8().constData()
                                    << bsoncxx::builder::stream::finalize,
                                  bsoncxx::builder::stream::document{}
                                    << "$set" << bsoncxx::builder::stream::open_document
                                    << "password" << hashedPsw.toUtf8().constData()
                                    << bsoncxx::builder::stream::close_document
                                    << bsoncxx::builder::stream::finalize);
    } catch (mongocxx::bulk_write_exception e){
        return false;
    }
    catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::changePassword] update_one error, connection to db failed.\nServer closed.";
        exit(EXIT_FAILURE);
    }

    return true;
}


QByteArray DatabaseManager::getImage(bsoncxx::document::view queryResult){
    bsoncxx::document::element imageElement = queryResult["image"];

    long size = imageElement.length();
    uint8_t *appo = const_cast<uint8_t*>(imageElement.get_binary().bytes);

    QByteArray imageToReturn;
    for (int i=0; i<size; i++) {
        imageToReturn.append(static_cast<char>(appo[i]));
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

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult;
    try{
        queryResult = userCollection.find_one(userToCheckView);
    } catch (mongocxx::query_exception e){
        qDebug() << "[ERROR][DatabaseManager::checkPassword] find_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

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


// documentName is unique globally
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
    } catch (mongocxx::bulk_write_exception e) {
        // fail, there is already a document with that name
        return false;
    }
    catch (mongocxx::query_exception e){
        qDebug() << "[ERROR][DatabaseManager::insertNewDocument] insert_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
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
    } catch (mongocxx::bulk_write_exception e) {
        // in case of fail, it means that the user still has the permission
        // for that  document, so we can simply ignore the exception
        return false;
    }
    catch (mongocxx::query_exception e){
        qDebug() << "[ERROR][DatabaseManager::insertNewPermission] insert_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
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

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult;

    try{
        queryResult= documentCollection.find_one(documentToGetView);
    } catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::getUri] find_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();
        bsoncxx::document::element uri = a["uri"];
        bsoncxx::stdx::string_view uriView = uri.get_utf8().value;
        QString appo = QString::fromStdString( uriView.to_string() );
        return appo;
    }
    else {
        // do nothing, I will arrive here only if the documentName
        // does not exist in db. But this will never happen, because
        // the name is passed via code, starting from the exiting files.
        return QString("");     //this line could be omitted
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

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult;
    try{
        queryResult = documentCollection.find_one(documentToGetView);
    } catch(mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::getDocument] find_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

    if(queryResult){
        bsoncxx::document::view a = (*queryResult).view();
        bsoncxx::document::element nameDocument = a["_id"];
        bsoncxx::stdx::string_view nameDocumentView = nameDocument.get_utf8().value;
        QString appo = QString::fromStdString( nameDocumentView.to_string() );
        return appo;
    }
    else {
        // the find_one query found nothing
        return QString("");
    }
}


void DatabaseManager::insertSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition,
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
                            << "alignment"  << alignment
                            << bsoncxx::builder::stream::close_document
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view symbolToInsertView = symbolToInsert.view();

    try {
        insertCollection.insert_one(symbolToInsertView);
    } catch (mongocxx::query_exception e) {
        qDebug() << "[ERROR][DatabaseManager::insertSymbol] insert_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

    this->incrementCounterOfCollection("insert");
}


//A symbol is uniquely identified by his fractional position and belonging document,
//so "symbol parameter" is useful, we pass it only for sake of completeness
void DatabaseManager::deleteSymbol(QString& nameDocument, QString& symbol, int siteIdOfSymbol, std::vector<int>& fractionalPosition){
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
    } catch (mongocxx::query_exception e) {
        qDebug() << "[ERROR][DatabaseManager::deleteSymbol] delete_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

}


QList<Char> DatabaseManager::getAllInserts(QString& nameDocument){
    QList<Char> orderedChars;
    mongocxx::collection insertCollection = (*this->db)["insert"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value insertsToRetrieve =
        elementBuilder << "nameDocument" << nameDocument.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view insertsToRetrieveView = insertsToRetrieve.view();

    // notice that the only instruction that should be included in try-catch
    // is insertCollection.find, but we do in this way because of scope problems.
    // Despite everything, we are sure that this is the only method that can raise
    // the below type of exception in catch().
    try{
        mongocxx::cursor resultIterator = insertCollection.find(insertsToRetrieveView);

        //(.1)Save them in local before ordering
        for (auto elem : resultIterator) {
            QString insert = QString::fromStdString(bsoncxx::to_json(elem));
            QJsonDocument stringDocJSON = QJsonDocument::fromJson(insert.toUtf8());

            QJsonObject insertObjJson = stringDocJSON.object();

            Char charToInsert = Char::fromJson(insertObjJson);

            orderedChars.push_back(charToInsert);
        }
    } catch (mongocxx::query_exception) {
        qDebug() << "[ERROR][DatabaseManager::getAllInserts] find error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

    //(.2)Now ordering.
    //It orders according to the order established in Char object,
    //so will be returned a vector in fractionalPosition ascending order
    std::sort(orderedChars.begin(), orderedChars.end());
    qDebug() << "Vettore di fractionalPos: ";
    std::for_each(orderedChars.begin(), orderedChars.end(), [](Char c){
        qDebug() << c.getFractionalPosition();
    });
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

    // notice that the only instruction that should be included in try-catch
    // is insertCollection.find, but we do in this way because of scope problems.
    // Despite everything, we are sure that this is the only method that can raise
    // the below type of exception in catch().
    try{
        mongocxx::cursor resultIterator = documentCollection.find(accountsToRetrieveView);

        for(auto elem : resultIterator) {
            bsoncxx::document::element siteIdAccount = elem["_id"]["siteId"];
            int siteId = siteIdAccount.get_int32().value;
            siteIdAccounts.append(siteId);
        }
    } catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::getAllAccounts] find error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
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

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult;
    try{
        queryResult = userCollection.find_one(accountToRetrieveView);
    } catch (mongocxx::query_exception){
        qDebug() << "[ERROR][DatabaseManager::getAccount] find_one error, connection to db failed.\nServer closed.";
        exit(EXIT_SUCCESS);
    }

    bsoncxx::document::view a = (*queryResult).view();

    QString accountString = QString::fromStdString(bsoncxx::to_json(a));
    QJsonDocument stringDocJSON = QJsonDocument::fromJson(accountString.toUtf8());

    QJsonObject accountObjJson = stringDocJSON.object();
    Account accountToReturn = Account::fromJson(accountObjJson, true);
    return accountToReturn;
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

