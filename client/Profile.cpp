#include <QIcon>
#include "Profile.h"
#include "ui_profile.h"

Profile::Profile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);
}

Profile::~Profile()
{
    delete ui;
}

void Profile::setUsername(const QString& username){
    ui->username->setText(username);
}

void Profile::setImagePic(const QPixmap &imagePic){
    QIcon profile_pic(imagePic);
    ui->image_pic->setPixmap(profile_pic.pixmap(QSize(10,10)));
}
