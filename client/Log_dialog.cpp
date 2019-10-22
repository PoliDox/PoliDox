#include "Log_dialog.h"
#include "ui_log_dialog.h"
#include <QMessageBox>
#include <QDebug>
#include <QListWidget>
#include <iostream>

Log_Dialog::Log_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_Dialog)
{
    ui->setupUi(this);
    this->setFixedSize(374,575);
    //this->ui->logo->setStyleSheet("background-image: url(./logo.png);background-repeat:none;background-position:center;");

    //this->ui->pushButton_register->setStyleSheet("color:crimson");
    //this->ui->pushButton_login->setStyleSheet("background-color:#4d79ff;color:white");

    //this->ui->line->setStyleSheet("background-color:#003879");
    //this->ui->label->setStyleSheet( "background-color:transparent;color:#003879");
    //this->ui->label_2->setStyleSheet( "background-color:transparent;color:#003879");

    //this->ui->lineEdit_username->setStyleSheet( "background-color:transparent");
    //this->ui->lineEdit_password->setStyleSheet( "background-color:transparent");
    setWindowTitle("PoliDox");

    lf = new ListFiles();

    /*AGGANCIARE L' EDITOR IN CASO DI NEW, VEDERE PERCHE' NON PARTE*/
    /*connect(lf, &ListFiles::makeNewEditor_fromNew, this, [&](QString filename){
        setEditor(new Editor());
    });*/
}

Log_Dialog::~Log_Dialog()
{
    delete ui;
}

void Log_Dialog::setEditor(Editor *editor){
    this->editor = editor;
}

/* METODO SPOSTATO IN LISTFILES */
void Log_Dialog::displayFiles(const QList<QString> p_files)
{
    // TODO: replace "SignIn" and "Register" button with a box including all available files
    // When a file is selected, emit the signal onFileSelected

    QObjectList list=this->ui->groupBox->children();

    for(auto it=list.begin();it!=list.end();it++)
        delete *it;

    this->ui->groupBox->setFixedSize(330,400);

    QListWidget* files=new QListWidget(this->ui->groupBox);
    connect(files,&QListWidget::itemDoubleClicked,this,&Log_Dialog::onClickedFile);
    QString file1("Create new file");
    QString file2("file2");
    QString file3("file3");
    QString file4("file4");
    files->addItem(file1);
    files->addItem(file2);
    files->addItem(file3);
    files->addItem(file4);

    //files->addItems(p_files);
    QVBoxLayout* layout = new QVBoxLayout();
    auto grid_layout = new QGridLayout(this->ui->groupBox);
    grid_layout->addWidget(files);
    files->show();


}

void Log_Dialog::on_pushButton_login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    emit authDataSubmitted(username, password);

}

void Log_Dialog::on_pushButton_register_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    emit signupDataSubmitted(username, password);
}

void Log_Dialog::onClickedFile(QListWidgetItem* item){

    std::cout << "SELECTED FILE: "<< item->text().toUtf8().constData() << std::endl;

    if(item->text()=="Creaete new file")
        emit newFileSelected(item->text());
    else
        emit fileSelected(item->text());

}
