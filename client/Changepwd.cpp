#include "Changepwd.h"
#include "ui_changepwd.h"
#include "qdebug.h"

ChangePwd::ChangePwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePwd)
{
    ui->setupUi(this);
}

ChangePwd::~ChangePwd()
{
    delete ui;
}

void ChangePwd::on_buttonBox_accepted()
{
    QString pwd = ui->lineEdit->text();
    //todo: fare controllo se string vuota
    //inviare messaggio al server
    this->hide();
}

void ChangePwd::on_buttonBox_rejected()
{
    this->hide();
}
