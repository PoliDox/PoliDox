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


double DatabaseManager::getCounterOfCollection(QString nameCollection){
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
        return element.get_double().value;
    }
    else{
        return (double)-1;
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
bool DatabaseManager::registerUser(QString name, QString password, QByteArray image){
    mongocxx::collection userCollection = (*this->db)["user"];

    double siteId = this->getCounterOfCollection("user");

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

    std::vector<unsigned char> imageVector(image.begin(), image.end());
    auto array_builder = bsoncxx::builder::basic::array{};
    for (unsigned char element : imageVector) {
        array_builder.append(element);
    }

    //ATTENZIONE: per ora, qui sotto, l'inserimento dell'immagine è commentato
    //            discommentare e testare!!!
    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userToInsert =
        elementBuilder << "_id"      << name.toUtf8().constData()
                       << "password" << hashedPsw.toUtf8().constData()
                       << "siteId"   << siteId
                       //<< "image"    << array_builder
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view userToInsertView = userToInsert.view();

    try {
        userCollection.insert_one(userToInsertView);
    } catch (std::exception e) {
        //caso utente con "name" già presente
        return false;
    }

    this->incrementCounterOfCollection("user");
    return true;
}


bool DatabaseManager::checkPassword(QString name, QString password){
    mongocxx::collection userCollection = (*this->db)["user"];

    QString hashedPsw = QCryptographicHash::hash((password.toUtf8()), QCryptographicHash::Md5).toHex();

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userToCheck =
        elementBuilder  << "_id"      << name.toUtf8().constData()
                        << "password" << hashedPsw.toUtf8().constData()
                        << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view userToCheckView = userToCheck.view();

    bsoncxx::stdx::optional<bsoncxx::document::value> queryResult = userCollection.find_one(userToCheckView);
    if(queryResult)
        return true;
    else
        return false;
}


//TODO: - sistemare bene l'eccezione
bool DatabaseManager::insertNewDocument(QString documentName){
    mongocxx::collection documentCollection = (*this->db)["document"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value docToInsert =
        elementBuilder << "_id" << documentName.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view docToInsertView = docToInsert.view();

    try {
        documentCollection.insert_one(docToInsertView);
    } catch (std::exception e) {
        //caso documento con "documentName" già presente
        return false;
    }
    return true;
}


//TODO: - sistemare il valore di ritorno
//      - implementare il vincolo di integrità su nameDocument
bool DatabaseManager::insertSymbol(QString nameDocument, QString symbol, std::vector<int> fractionalPosition) {
    mongocxx::collection insertCollection = (*this->db)["insert"];

    double counterInsert = this->getCounterOfCollection("insert");

    auto array_builder = bsoncxx::builder::basic::array{};
    for (int element : fractionalPosition) {
        array_builder.append(element);
    }

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value symbolToInsert =
        elementBuilder << "_id"                << counterInsert
                       << "nameDocument"       << nameDocument.toUtf8().constData()
                       << "symbol"             << symbol.toUtf8().constData()
                       << "fractionalPosition" << array_builder
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
bool DatabaseManager::deleteSymbol(QString nameDocument, QString symbol, std::vector<int> fractionalPosition){
    mongocxx::collection insertCollection = (*this->db)["insert"];

    auto array_builder = bsoncxx::builder::basic::array{};
    for (int element : fractionalPosition) {
        array_builder.append(element);
    }

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value symbolToDelete =
        elementBuilder << "nameDocument"       << nameDocument.toUtf8().constData()
                       << "symbol"             << symbol.toUtf8().constData()
                       << "fractionalPosition" << array_builder
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


//TODO: - anziché il for da stampare in output, capire
//        bene cosa restituire. Un vector<Char> su cui
//        poi verranno chiamate le remoteInsert??
void DatabaseManager::retrieveAllInserts(QString nameDocument){
    mongocxx::collection insertCollection = (*this->db)["insert"];

    auto elementBuilder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value insertsToRetrieve =
        elementBuilder << "nameDocument" << nameDocument.toUtf8().constData()
                       << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view insertsToRetrieveView = insertsToRetrieve.view();

    //settings needed to order the result by fractionalPosition
    auto order = bsoncxx::builder::stream::document{} << "fractionalPosition" << 1 << bsoncxx::builder::stream::finalize;
    auto opts = mongocxx::options::find{};
    opts.sort(order.view());

    mongocxx::cursor resultIterator = insertCollection.find(insertsToRetrieveView, opts);

    //DA QUI IN POI E' ANCORA DA IMPLEMENTARE
    //TOGLIERE IL FOR QUI SOTTO
    for (auto elem : resultIterator) {
        std::cout << bsoncxx::to_json(elem) << std::endl;
    }
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

