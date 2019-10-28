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
    this->ui->logo->setStyleSheet("background-image: url(./logo.png);background-repeat:none;background-position:center;");

    this->ui->pushButton_register->setStyleSheet("color:crimson");
    //this->ui->pushButton_login->setStyleSheet("background-color:#4d79ff;color:white");

    this->ui->line->setStyleSheet("background-color:#003879");
    this->ui->label->setStyleSheet( "background-color:transparent;color:#003879");
    this->ui->label_2->setStyleSheet( "background-color:transparent;color:#003879");

    this->ui->lineEdit_username->setStyleSheet( "background-color:transparent");
    this->ui->lineEdit_password->setStyleSheet( "background-color:transparent");
    setWindowTitle("PoliDox");

    lf = new ListFiles();
    nfd = new NewFileDialog(this);

    connect(nfd, &NewFileDialog::getFileName, this, [&](QString newfilename){
        emit newFileSelected(newfilename);
    });

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

    QString new_file("Create new file");
    files->addItem(new_file);
    files->addItems(p_files);

    QGridLayout* grid_layout = new QGridLayout(this->ui->groupBox);
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

    /*QSizePolicy sp_retain = ui->groupBox->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->groupBox->setSizePolicy(sp_retain);*/

    hideLoginForm();

    createRegistrationForm();

}

void Log_Dialog::manageRegistrationData(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    cleanRegistrationForm();

    ui->groupBox->setFixedSize(335,265);

    showLoginForm();

    ui->lineEdit_username->setText(username);
    ui->lineEdit_password->setText(password);

    //QVBoxLayout* vertical_layout=static_cast<QVBoxLayout*>(ui->groupBox->layout());

    emit signupDataSubmitted(username, password);



}

void Log_Dialog::showLoginForm(){

    QList<QWidget*> list=this->ui->groupBox->findChildren<QWidget*>();

    for(auto it=list.begin();it!=list.end();it++)
            (*it)->show();
}

void Log_Dialog::hideLoginForm(){

    QList<QWidget*> list=ui->groupBox->findChildren<QWidget*>();

    for(auto it=list.begin();it!=list.end();it++)
            (*it)->hide();

}

void Log_Dialog::createRegistrationForm(){

    ui->groupBox->setFixedSize(330,400);


    QLabel* name=new QLabel("Name",ui->groupBox);
    QLabel* surname=new QLabel("Surname",ui->groupBox);
    QLabel* usr=new QLabel("Username",ui->groupBox);
    QLabel* pwd=new QLabel("Password",ui->groupBox);

    name->setObjectName("name");
    surname->setObjectName("surname");
    usr->setObjectName("user");
    pwd->setObjectName("pwd");

    name->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    surname->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    usr->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    pwd->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");

    QLineEdit* name_form=new QLineEdit(ui->groupBox);
    QLineEdit* surname_form=new QLineEdit(ui->groupBox);
    QLineEdit* usr_form=new QLineEdit(ui->groupBox);
    QLineEdit* pwd_form=new QLineEdit(ui->groupBox);

    name_form->setStyleSheet("background-color:transparent;");
    surname_form->setStyleSheet("background-color:transparent;");
    usr_form->setStyleSheet("background-color:transparent;");
    pwd_form->setStyleSheet("background-color:transparent;");

    name_form->setObjectName("name_line");
    surname_form->setObjectName("surname_line");
    usr_form->setObjectName("user_line");
    pwd_form->setObjectName("pwd_line");

    name_form->setPlaceholderText("Insert your name");
    surname_form->setPlaceholderText("Insert your surname");
    usr_form->setPlaceholderText("Insert your username");
    pwd_form->setPlaceholderText("Insert your password");

    QPushButton* submit=new QPushButton("Submit",ui->groupBox);
    QPushButton* cancel=new QPushButton("Cancel");

    submit->setObjectName("submit");
    cancel->setObjectName("cancel");

    QGridLayout* grid_layout = static_cast<QGridLayout*>(ui->groupBox->layout());

    grid_layout->setVerticalSpacing(0);

    name->setFixedSize(300,30);
    surname->setFixedSize(300,30);
    usr->setFixedSize(300,30);
    pwd->setFixedSize(300,30);
    submit->setFixedSize(100,30);

    grid_layout->addWidget(name,0,0,0);
    grid_layout->addWidget(name_form,1,0,0);
    grid_layout->addWidget(surname,2,0,0);
    grid_layout->addWidget(surname_form,3,0,0);
    grid_layout->addWidget(usr,4,0,0);
    grid_layout->addWidget(usr_form,5,0,0);
    grid_layout->addWidget(pwd,6,0,0);
    grid_layout->addWidget(pwd_form,7,0,0);
    grid_layout->addWidget(submit,9,0,0);
    grid_layout->addWidget(cancel,9,1,0);


    connect(submit,&QPushButton::clicked,this,&Log_Dialog::manageRegistrationData);

    connect(cancel,&QPushButton::clicked,this,&Log_Dialog::cleanRegistrationForm);

}
void Log_Dialog::cleanRegistrationForm(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));
    QLineEdit* name_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("name_line"));
    QLineEdit* surname_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("surname_line"));

    QLabel* name=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("name"));
    QLabel* surname=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("surname"));
    QLabel* user=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("user"));
    QLabel* pwd=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("pwd"));

    QPushButton* submit=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("submit"));
    QPushButton* cancel=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("cancel"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    delete user_linedit;
    delete pwd_linedit;
    delete name_linedit;
    delete surname_linedit;

    delete name;
    delete surname;
    delete user;
    delete pwd;

    delete submit;
    delete cancel;

    ui->groupBox->setFixedSize(335,265);

    QList<QWidget*> list=this->ui->groupBox->findChildren<QWidget*>();
    for(auto it=list.begin();it!=list.end();it++)
            (*it)->show();

    ui->lineEdit_username->setText(username);
    ui->lineEdit_password->setText(password);

    //QVBoxLayout* vertical_layout=static_cast<QVBoxLayout*>(ui->groupBox->layout());


}

void Log_Dialog::onClickedFile(QListWidgetItem* item){

    qDebug() << "SELECTED FILE: "<< item->text();

    if(item->text().compare("Create new file") == 0){
        nfd->show();
    }
    else{
        this->hide();
        emit fileSelected(item->text());
    }

}
