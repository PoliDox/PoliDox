#include <QIcon>
#include "Profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);
    this->ChangeImgDialog = new ChangeImg(this);
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
    //todo
}

void Profile::on_changeImage_clicked()
{
    //todo
}
