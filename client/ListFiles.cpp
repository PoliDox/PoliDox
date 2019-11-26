#include "ListFiles.h"
#include "ui_listfiles.h"
#include <QDebug>

ListFiles::ListFiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListFiles)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color:#d0e0e2;");
    setWindowTitle("PoliDox");
    nfd = new NewFileDialog();

    /*
    connect(nfd, &NewFileDialog::startNewFileEditor, this, [&](QString filename){
        createNewEditor_fromNew(filename);
    });
    */
}

ListFiles::~ListFiles()
{
    delete ui;
}

void ListFiles::createNewEditor_fromNew(QString filename){
    emit makeNewEditor_fromNew(filename);   //CAPIRE PERCHE' QUESTA EMIT NON ARRIVA
    hide();
}

void ListFiles::displayFiles(const QList<QString> p_files)
{
    std::for_each(p_files.begin(), p_files.end(), [&](QString file_i){ ui->listWidget->addItem(file_i); });
}

/* creazione nuovo file */
void ListFiles::on_NewFile_clicked()
{

    nfd->show();
    QString fileName = nfd->getNewFileName();
    ui->listWidget->addItem(fileName);
}

/* apertura file selezionato */
void ListFiles::on_Open_clicked()
{

}

/* cancellazione file selezionato */
void ListFiles::on_Delete_clicked()
{

}

/* generazione link per il file selezionato */
void ListFiles::on_GenerateLink_clicked()
{

}
