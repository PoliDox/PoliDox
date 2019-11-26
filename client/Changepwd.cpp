#include "Changepwd.h"
#include "ui_changepwd.h"
#include <QMessageBox>

ChangePwd::ChangePwd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangePwd)
{
    ui->setupUi(this);
    setWindowTitle("PoliDox");
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
        emit PwdUpdate(pwd);
    }
    this->hide();
}

void ChangePwd::on_buttonBox_rejected()
{
    this->hide();
}
