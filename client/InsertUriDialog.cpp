#include "InsertUriDialog.h"
#include "ui_inserturidialog.h"
#include <QMessageBox>

InsertUriDialog::InsertUriDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertUriDialog)
{
    ui->setupUi(this);
    setWindowTitle("Insert Uri");
}

InsertUriDialog::~InsertUriDialog()
{
    delete ui;
}

void InsertUriDialog::on_buttonBox_accepted()
{
    if(ui->inputUri->text().isEmpty()){
        QMessageBox::warning(this, "Uri", "Please insert a valid Uri");
    }else{
        uriName = ui->inputUri->text();
        emit getUriName(uriName);
    }
}

