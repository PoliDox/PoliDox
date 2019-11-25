#include "Changepwd.h"
#include "ui_changepwd.h"

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
