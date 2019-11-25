#include "Changepwd.h"
#include "ui_changepwd.h"
#include <QMessageBox>

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
    if ( pwd.size() == 0 ){
        QMessageBox::warning(this, "PoliDox", "Password not valid");
    } else {
        QMessageBox::information(this, "PoliDox", "Password correctly updated");
    }
    this->hide();
    //TODO: inviare messaggio al server con la nunova password
}

void ChangePwd::on_buttonBox_rejected()
{
    this->hide();
}
