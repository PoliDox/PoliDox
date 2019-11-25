#include "Changeimg.h"
#include "ui_changeimg.h"
#include "qdebug.h"

ChangeImg::ChangeImg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeImg)
{
    ui->setupUi(this);
}

ChangeImg::~ChangeImg()
{
    delete ui;
}

void ChangeImg::on_buttonBox_accepted()
{
    //todo: inviare messaggio immagine

    this->hide();
}

void ChangeImg::on_buttonBox_rejected()
{
    this->hide();
}
