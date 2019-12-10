#include "LoginWindow.h"
#include "ui_loginwindow.h"
#include "ClientMessageFactory.h"
#include <QMessageBox>
#include <QDebug>
#include <QListWidget>
#include <iostream>
#include <QFileDialog>
#include <QComboBox>
#include <QScrollArea>
#include <QSizePolicy>


LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    this->setFixedSize(374,575);
    this->ui->logo->setStyleSheet("background-image: url(://images/images/logo.png);background-repeat:none;background-position:center;");

    setWindowIcon(QIcon("://images/images/logo_small.png"));

    this->ui->pushButton_register->setStyleSheet("color:crimson");

    this->ui->line->setStyleSheet("background-color:#003879");
    this->ui->label->setStyleSheet( "background-color:transparent;color:#003879");
    this->ui->label_2->setStyleSheet( "background-color:transparent;color:#003879");

    this->ui->lineEdit_username->setStyleSheet( "background-color:transparent");
    this->ui->lineEdit_password->setStyleSheet( "background-color:transparent");
    setWindowTitle("PoliDox");

    try{
        nfd = new NewFileDialog(this);
        uriDialog = new InsertUriDialog(this);
    } catch (std::bad_alloc& e){
        qDebug() << "[ERROR][LoginWindow::LoginWindow] new operator error." << e.what();
        throw;
    }

    connect(nfd, &NewFileDialog::getFileName, this, [&](QString& newfilename){
        emit newFileSelected(newfilename);
    });

    connect(uriDialog, &InsertUriDialog::getUriName, this, [&](QString& uri) {
       emit uriSelected(uri);
    });

}

LoginWindow::~LoginWindow()
{
    delete ui;
    delete nfd;
    delete uriDialog;
}


void LoginWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closed();
}


void LoginWindow::displayFiles(const QList<QString>& p_files)
{
    QObjectList list=this->ui->groupBox->children();

    for(auto it=list.begin();it!=list.end();it++)
        delete *it;

    this->ui->groupBox->setFixedSize(330,400);

    QListWidget* files=new QListWidget(this->ui->groupBox);
    connect(files,&QListWidget::itemDoubleClicked,this,&LoginWindow::onClickedFile);

    QString new_file("Create new file");
    QString fileFromUri("Insert URI");

    QListWidgetItem *separator = nullptr;
    try{
        separator = new QListWidgetItem();
    } catch(std::bad_alloc& e){
        qDebug() << "[ERROR][LoginWindow::displayFiles] new operator error on QListWidgetItem. " << e.what();
        throw;
    }

    separator->setSizeHint(QSize(300, 15));
    separator->setFlags(Qt::NoItemFlags);
    files->addItem(new_file);
    files->addItem(fileFromUri);
    files->addItem(separator);

    QFrame *frame = nullptr;
    try{
        frame = new QFrame();
    } catch(std::bad_alloc& e){
        qDebug() << "[ERROR][LoginWindow::displayFiles] new operator error on QFrame. " << e.what();
        throw;
    }
    frame->setFrameShape(QFrame::HLine);
    files->setItemWidget(separator, frame);
    files->addItems(p_files);

    QGridLayout* grid_layout = nullptr;
    try {
        grid_layout = new QGridLayout(this->ui->groupBox);
    } catch (std::bad_alloc& e) {
        qDebug() << "[ERROR][LoginWindow::displayFiles] new operator error on QGridLayout. " << e.what();
        throw;
    }
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
    QLabel* file_path=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_path"));


    QString username=user_linedit->text();
    QString password=pwd_linedit->text();
    QString imagePath=file_path->text();

    if ( password.size() == 0 ){
        QMessageBox::warning(this, "PoliDox", "Username and password cannot be empty");
    } else {        
        if (imagePath.size() == 0) {
            // If no image is provided we use the default image
            imagePath = QString("://images/images/no-pic.png");
        }
        QPixmap image(imagePath);
        emit signupDataSubmitted(username, password, image);
    }

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

    QLabel* usr = nullptr;
    QLabel* pwd = nullptr;
    try {
        usr = new QLabel("Username",ui->groupBox);
        pwd = new QLabel("Password",ui->groupBox);
    } catch (std::bad_alloc& e) {
        qDebug() << "[ERROR][LoginWindow::createRegistrationForm] new operator error on QLabel. " << e.what();
        throw;
    }

    usr->setObjectName("user");
    pwd->setObjectName("pwd");

    usr->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    pwd->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");

    QLineEdit* usr_form = nullptr;
    QLineEdit* pwd_form = nullptr;
    try {
        usr_form = new QLineEdit(ui->groupBox);
        pwd_form = new QLineEdit(ui->groupBox);
    } catch (std::bad_alloc& e) {
        qDebug() << "[ERROR][LoginWindow::createRegistrationForm] new operator error on QLineEdit. " << e.what();
        throw;
    }
    pwd_form->setEchoMode(QLineEdit::Password);

    usr_form->setStyleSheet("background-color:transparent;");
    pwd_form->setStyleSheet("background-color:transparent;");

    usr_form->setObjectName("user_line");
    pwd_form->setObjectName("pwd_line");

    usr_form->setPlaceholderText("Insert your username");
    pwd_form->setPlaceholderText("Insert your password");

    QPushButton* submit = nullptr;
    QPushButton* cancel = nullptr;
    try {
        submit = new QPushButton("Submit", ui->groupBox);
        cancel = new QPushButton("Cancel", ui->groupBox);
    } catch (std::bad_alloc& e) {
        qDebug() << "[ERROR][LoginWindow::createRegistrationForm] new operator error on QPushButton. " << e.what();
        throw;
    }

    submit->setObjectName("submit");
    cancel->setObjectName("cancel");

    QLabel* img_label = nullptr;
    QLabel* img_path = nullptr;
    QPushButton* img_selection = nullptr;
    QLabel* img_show = nullptr;
    QLabel* img_warning = nullptr;
    try {
        img_label = new QLabel("Profile pic", ui->groupBox);
        img_path = new QLabel(ui->groupBox);
        img_selection = new QPushButton("upload file", ui->groupBox);
        img_show = new QLabel(ui->groupBox);
        img_warning = new QLabel(ui->groupBox);
    } catch (std::bad_alloc& e) {
        qDebug() << "[ERROR][LoginWindow::createRegistrationForm] new operator error on QLabel | QPushButton. " << e.what();
        throw;
    }

    connect(img_selection, &QPushButton::clicked, this, &LoginWindow::upload_clicked);

    img_label->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    img_path->setStyleSheet("background-color:transparent;"); //border: 1px solid #8d918d
    img_label->setStyleSheet("background-color:transparent;\ncolor:#003879;font-weight:bold;font-family:Courier;font-size:16px");
    img_warning->setStyleSheet("color:red");

    img_warning->setText("Image must be smaller than 2MB");

    img_label->setObjectName("img_label");
    img_selection->setObjectName("img_selection");
    img_path->setObjectName("img_path");
    img_show->setObjectName("img_show");
    img_warning->setObjectName("img_warning");

    QGridLayout* grid_layout = static_cast<QGridLayout*>(ui->groupBox->layout());

    usr->setFixedSize(200,30);
    pwd->setFixedSize(200,30);
    img_label->setFixedSize(200,30);
    submit->setFixedSize(100,30);
    img_selection->setFixedSize(100,30);
    img_path->setFixedSize(300,30);

    QPixmap noPic("://images/images/no-pic.png");

    #ifdef Q_OS_LINUX
        img_show->setFixedSize(50,50);
        img_show->setPixmap(noPic.scaled(50,50));
    #else
        img_show->setFixedSize(100,100);
        img_show->setPixmap(noPic.scaled(100,100));
    #endif

    grid_layout->setHorizontalSpacing(0);
    grid_layout->setVerticalSpacing(5);

    grid_layout->addWidget(usr,0,0,nullptr);
    grid_layout->addWidget(usr_form,1,0,nullptr);
    grid_layout->addWidget(pwd,2,0,nullptr);
    grid_layout->addWidget(pwd_form,3,0,nullptr);
    grid_layout->addWidget(img_label,4,0,nullptr);
    grid_layout->addWidget(img_show,5,0,nullptr);
    grid_layout->addWidget(img_warning,6,0,nullptr);
    grid_layout->addWidget(img_selection,7,0,nullptr);
    grid_layout->addWidget(img_path,8,0,nullptr);

    QSpacerItem* spacer = new QSpacerItem(1,10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    grid_layout->addItem(spacer,9,0);

    grid_layout->addWidget(submit,10,0,nullptr);
    grid_layout->addWidget(cancel,10,1,nullptr);

    connect(submit,&QPushButton::clicked,this,&LoginWindow::sendRegistrationData);

    connect(cancel,&QPushButton::clicked,this,&LoginWindow::cleanRegistrationForm);
}


void LoginWindow::cleanRegistrationForm(){

    QLineEdit* user_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("user_line"));
    QLineEdit* pwd_linedit=static_cast<QLineEdit*>(ui->groupBox->findChild<QLineEdit*>("pwd_line"));

    QLabel* user=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("user"));
    QLabel* pwd=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("pwd"));
    QLabel* img_label=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_label"));
    QLabel* img_path=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_path"));
    QLabel* img_show=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_show"));
    QLabel* img_warning=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_warning"));

    QPushButton* submit=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("submit"));
    QPushButton* cancel=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("cancel"));
    QPushButton* img_selection=static_cast<QPushButton*>(ui->groupBox->findChild<QPushButton*>("img_selection"));

    QString username=user_linedit->text();
    QString password=pwd_linedit->text();

    QLayoutItem* spacer=ui->groupBox->layout()->takeAt(0);
    delete spacer;

    delete user_linedit;
    delete pwd_linedit;

    delete user;
    delete pwd;
    delete img_label;
    delete img_path;
    delete img_show;
    delete img_warning;

    delete img_selection;
    delete submit;
    delete cancel;

    ui->groupBox->setFixedSize(335,265);

    QList<QWidget*> list=this->ui->groupBox->findChildren<QWidget*>();
    for(auto it=list.begin();it!=list.end();it++)
            (*it)->show();

    ui->lineEdit_username->setText(username);
    ui->lineEdit_password->setText(password);
}

void LoginWindow::onClickedFile(QListWidgetItem* item){
    qDebug() << "SELECTED FILE: "<< item->text();

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

void LoginWindow::upload_clicked(){
    QFileDialog file_selection(ui->groupBox);
    file_selection.setNameFilter(tr("JPEG (*.jpg *.jpeg *.png)"));

    file_selection.show();

    QString filePath;
    if (file_selection.exec())
         filePath = file_selection.selectedFiles().at(0);

    QFile img(filePath);
    QLabel* img_warning=static_cast<QLabel*>(ui->groupBox->findChild<QLabel*>("img_warning"));

    if(img.size()>2000000){
        QMessageBox::warning(this, "ImgWarning", "The file's dimension is greater than 2MB!");
        img_warning->setText("Image must be smaller than 2MB");
        return;

    } else {
        img_warning->setStyleSheet("color:green");
        img_warning->setText("File respects dimension costraint");
    }

    if(filePath.size()>0){

        QLabel* img_path=ui->groupBox->findChild<QLabel*>("img_path");
        img_path->setText(filePath);

        QLabel* img_show=ui->groupBox->findChild<QLabel*>("img_show");

        QPixmap noPic(filePath);

        #ifdef Q_OS_LINUX
            img_show->setPixmap(noPic.scaled(50,50));
        #else
            img_show->setPixmap(noPic.scaled(100,100));
        #endif
    }

}
