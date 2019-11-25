#include <QIcon>
#include <QFileDialog>
#include <QMessageBox>
#include <qdebug.h>
#include "Profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);
    this->ChangePwdDialog = new ChangePwd(this);
}

Profile::~Profile()
{
    delete ui;
}

void Profile::setUsername(const QString& username){
    ui->username->setText(username);
}

void Profile::setImagePic(const QPixmap &imagePic){
    ui->image_pic->setPixmap(imagePic.scaled(200, 200));
}

void Profile::on_changePassword_clicked()
{
    this->ChangePwdDialog->show();
}

void Profile::on_changeImage_clicked()
{
    QFileDialog file_selection(ui->image_pic);
    file_selection.setNameFilter(tr("JPEG (*.jpg *.jpeg *.png)"));

    file_selection.show();

    QString filePath;
    if (file_selection.exec())
         filePath = file_selection.selectedFiles().at(0);

    QPixmap pix(filePath);
    QFile img(filePath);

    if(img.size()>10000000){
        QMessageBox::warning(this, "ImgWarning", "The file's dimension is greater than 10MB!");
        return;
    }

    if(filePath.size()>0){
        ui->image_pic->setPixmap(pix.scaled(200, 200));
        QMessageBox::information(this, "PoliDox", "Image correctly updated");
    }

    //TODO:inviare il messaggio al server con la nuova immagine
}
