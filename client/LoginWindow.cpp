#include "LoginWindow.h"
#include "ui_loginwindow.h"
#include "ClientMessageFactory.h"
#include <QMessageBox>
#include <QDebug>
#include <QListWidget>
#include <iostream>
#include <QFileDialog>
#include <QComboBox>


LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setFixedSize(374,575);
    this->ui->logo->setStyleSheet("background-image: url(://images/images/logo.png);background-repeat:none;background-position:center;");

    setWindowIcon(QIcon("://images/images/logo_small.png"));

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
    uriDialog = new InsertUriDialog(this);

    connect(nfd, &NewFileDialog::getFileName, this, [&](QString newfilename){
        emit newFileSelected(newfilename);
    });

    connect(uriDialog, &InsertUriDialog::getUriName, this, [&](QString uri) {
       emit uriSelected(uri);
    });

}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::setEditor(Editor *editor){
    this->editor = editor;
}

/* METODO SPOSTATO IN LISTFILES */
void LoginWindow::displayFiles(const QList<QString> p_files)
{
    // TODO: replace "SignIn" and "Register" button with a box including all available files
    // When a file is selected, emit the signal onFileSelected

    QObjectList list=this->ui->groupBox->children();

    for(auto it=list.begin();it!=list.end();it++)
        delete *it;

    this->ui->groupBox->setFixedSize(330,400);

    QListWidget* files=new QListWidget(this->ui->groupBox);
    connect(files,&QListWidget::itemDoubleClicked,this,&LoginWindow::onClickedFile);

    QString new_file("Create new file");
    QString fileFromUri("Insert URI");

    QListWidgetItem *separator = new QListWidgetItem();
    separator->setSizeHint(QSize(300, 15));
    separator->setFlags(Qt::NoItemFlags);
    files->addItem(new_file);
    files->addItem(fileFromUri);
    files->addItem(separator);
    QFrame *frame = new QFrame();
    frame->setFrameShape(QFrame::HLine);
    files->setItemWidget(separator, frame);
    files->addItems(p_files);

    QGridLayout* grid_layout = new QGridLayout(this->ui->groupBox);
    grid_layout->addWidget(files);
    files->show();


}

void LoginWindow::on_pushButton_login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    emit authDataSubmitted(username, password);

}

void LoginWindow::on_pushButton_register_clicked()
{

    /*QSizePolicy sp_retain = ui->groupBox->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->groupBox->setSizePolicy(sp_retain);*/

    hideLoginForm();

    createRegistrationForm();

}


void LoginWindow::registrationOk(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    cleanRegistrationForm();

    ui->groupBox->setFixedSize(335,265);

    showLoginForm();

    ui->lineEdit_username->setText(username);
    ui->lineEdit_password->setText(password);

}


void LoginWindow::sendRegistrationData(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    //QVBoxLayout* vertical_layout=static_cast<QVBoxLayout*>(ui->groupBox->layout());


    // TODO: passa l'immagine profilo
    // Consider using the constructor QPixmap(const QString &fileName, const char *format = nullptr)
    emit signupDataSubmitted(username, password, QPixmap());

}

void LoginWindow::showLoginForm(){

    QList<QWidget*> list=this->ui->groupBox->findChildren<QWidget*>();

    for(auto it=list.begin();it!=list.end();it++)
            (*it)->show();
}

void LoginWindow::hideLoginForm(){

    QList<QWidget*> list=ui->groupBox->findChildren<QWidget*>();

    for(auto it=list.begin();it!=list.end();it++)
            (*it)->hide();

}

void LoginWindow::createRegistrationForm(){

    ui->groupBox->setFixedSize(330,400);

    QLabel* usr=new QLabel("Username",ui->groupBox);
    QLabel* pwd=new QLabel("Password",ui->groupBox);

    usr->setObjectName("user");
    pwd->setObjectName("pwd");

    usr->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    pwd->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");

    QLineEdit* usr_form=new QLineEdit(ui->groupBox);
    QLineEdit* pwd_form=new QLineEdit(ui->groupBox);
    pwd_form->setEchoMode(QLineEdit::Password);

    usr_form->setStyleSheet("background-color:transparent;");
    pwd_form->setStyleSheet("background-color:transparent;");

    usr_form->setObjectName("user_line");
    pwd_form->setObjectName("pwd_line");

    usr_form->setPlaceholderText("Insert your username");
    pwd_form->setPlaceholderText("Insert your password");

    QPushButton* submit=new QPushButton("Submit",ui->groupBox);
    QPushButton* cancel=new QPushButton("Cancel");


    submit->setObjectName("submit");
    cancel->setObjectName("cancel");

    QLabel* img_label=new QLabel("Profile pic",ui->groupBox);
    QLabel* img_path=new QLabel("No file selected",ui->groupBox);
    QPushButton* img_selection=new QPushButton("upload file",ui->groupBox);
    QLabel* img_show=new QLabel(ui->groupBox);

    connect(img_selection,&QPushButton::clicked,this,&LoginWindow::upload_clicked);

    img_label->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    img_path->setStyleSheet("background-color:transparent;border: 1px solid #8d918d");
    img_label->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    //img_show->setStyleSheet("background-color:navy;");

    img_label->setObjectName("img_label");
    img_selection->setObjectName("img_selection");
    img_path->setObjectName("img_path");
    img_show->setObjectName("img_show");



    QGridLayout* grid_layout = static_cast<QGridLayout*>(ui->groupBox->layout());


    usr->setFixedSize(200,30);
    pwd->setFixedSize(200,30);
    img_label->setFixedSize(200,30);
    submit->setFixedSize(100,30);
    img_selection->setFixedSize(100,30);
    img_path->setFixedSize(200,30);
    img_show->setFixedSize(100,100);

    grid_layout->setHorizontalSpacing(0);
    grid_layout->setVerticalSpacing(0);

    grid_layout->addWidget(usr,0,0,nullptr);
    grid_layout->addWidget(usr_form,1,0,nullptr);
    grid_layout->addWidget(pwd,2,0,nullptr);
    grid_layout->addWidget(pwd_form,3,0,nullptr);
    grid_layout->addWidget(img_label,4,0,nullptr);
    grid_layout->addWidget(img_path,5,0,nullptr);
    grid_layout->addWidget(img_show,5,1,nullptr);
    grid_layout->addWidget(img_selection,6,0,nullptr);

    grid_layout->addWidget(submit,7,0,nullptr);
    grid_layout->addWidget(cancel,7,1,nullptr);


    connect(submit,&QPushButton::clicked,this,&LoginWindow::sendRegistrationData);

    connect(cancel,&QPushButton::clicked,this,&LoginWindow::cleanRegistrationForm);

}
void LoginWindow::cleanRegistrationForm(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));

    QLabel* user=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("user"));
    QLabel* pwd=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("pwd"));

    QPushButton* submit=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("submit"));
    QPushButton* cancel=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("cancel"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    delete user_linedit;
    delete pwd_linedit;

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

void LoginWindow::onClickedFile(QListWidgetItem* item){

    //qDebug() << "SELECTED FILE: "<< item->text();

    if(item->text().compare("Create new file") == 0){
        nfd->show();
    }else if(item->text().compare("Insert URI") == 0){
        uriDialog->show();
    }
    else{
        this->hide();
        emit fileSelected(item->text());
    }

}

void LoginWindow::upload_clicked(bool checked){

    QFileDialog* file_selection=new QFileDialog(ui->groupBox);
    file_selection->setNameFilter(tr("JPEG (*.jpg *.jpeg *.png)"));

    file_selection->show();

    QString filePath;
    if (file_selection->exec())
         filePath = file_selection->selectedFiles().at(0);

    QList<QString>splitted=filePath.split("/");
    QString fileName=splitted.at(splitted.size()-1);

    QLabel* img_path=ui->groupBox->findChild<QLabel*>("img_path");
    img_path->setText(fileName);

    delete file_selection;

     QLabel* img_show=ui->groupBox->findChild<QLabel*>("img_show");
     img_show->setStyleSheet("border-image: url("+filePath+") 0 0 0 0 stretch stretch;");

}
