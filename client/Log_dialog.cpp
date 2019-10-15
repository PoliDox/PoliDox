#include "Log_dialog.h"
#include "ui_log_dialog.h"
#include <QMessageBox>

Log_Dialog::Log_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Log_Dialog)
{
    ui->setupUi(this);
    setWindowTitle("PoliDox");
}

Log_Dialog::~Log_Dialog()
{
    delete ui;
}

void Log_Dialog::setEditor(Editor *editor){
    this->editor = editor;
}

void Log_Dialog::on_pushButton_login_clicked()
{
    QString username = ui->lineEdit_username->text();
    QString password = ui->lineEdit_password->text();

    /* TODO: implementare l' autenticazione con il database */

    /* stub di accesso - verra` sostituito con l' autenticazione al DB */
    if(username == "minnoz" && password == "piedi") {
        QMessageBox::information(this, "Login", "Welcome back to PoliDox, " + username + "!");
        editor->show();
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
