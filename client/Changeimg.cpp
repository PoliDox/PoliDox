#include "Changeimg.h"
#include "ui_changeimg.h"

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
