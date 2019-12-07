#include "Creators.h"
#include "ui_creators.h"

Creators::Creators(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Creators)
{
    ui->setupUi(this);
    QPixmap Logo("://images/images/logo.png");
    ui->Logo->setPixmap(Logo);
    setWindowTitle("PoliDox Creators");
}

Creators::~Creators()
{
    delete ui;
}
