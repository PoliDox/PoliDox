#include "NewFileDialog.h"
#include "ui_newfiledialog.h"
#include <QMessageBox>

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);
    setWindowTitle("New file");
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

void NewFileDialog::on_buttonBox_accepted()
{
    if(ui->inputName->text().isEmpty()){
        QMessageBox::warning(this, "New File", "Please insert the file name");
    }else{
        newFileName = ui->inputName->text();
        emit getFileName(newFileName);
    }
}

QString NewFileDialog::getNewFileName(){
    return newFileName;
}
