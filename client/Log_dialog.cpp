#include "Log_dialog.h"
#include "ui_log_dialog.h"
#include <QMessageBox>
#include <QDebug>

Log_Dialog::Log_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_Dialog)
{
    ui->setupUi(this);
    //this->setStyleSheet("background-color:#d0e0e2;");
    this->ui->logo->setStyleSheet("background-image: url(./logo.png);background-repeat:none;background-position:center;");

    this->ui->pushButton_register->setStyleSheet("color:crimson");
    this->ui->pushButton_login->setStyleSheet("border:1px");

    this->ui->line->setStyleSheet("background-color:#003879");
    this->ui->label->setStyleSheet( "background-color:transparent;color:#003879");
    this->ui->label_2->setStyleSheet( "background-color:transparent;color:#003879");

    this->ui->lineEdit_username->setStyleSheet( "background-color:transparent");
    this->ui->lineEdit_password->setStyleSheet( "background-color:transparent");
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
}

void Log_Dialog::on_pushButton_login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();   

    emit authDataSubmitted(username, password);

    /* stub di accesso - verra` sostituito con l' autenticazione al DB */

    if(username == "minnoz" && password == "piedi") {
        QMessageBox::information(this, "Login", "Welcome back to PoliDox, " + username + "!");
        //setEditor(new Editor());
        lf->show();
        //editor->show();
        hide();
    }
    else {
        QMessageBox::warning(this,"Login", "Username and password is not correct");
    }

}

void Log_Dialog::on_pushButton_register_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    /* IMPLEMENTARE LA REGISTRAZIONE DELL' UTENTE NEL DATABASE */

    /* IF SUCCESS
     * register
     * editor->show();
     * hide();
    //ELSE
     * QMessageBox::warning(this,"Login", "error message here");
     */
}
