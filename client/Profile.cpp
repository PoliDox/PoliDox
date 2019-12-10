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
    this->changePwdDialog = new ChangePwd(this);
    setWindowTitle("Profile");

    connect(changePwdDialog, &ChangePwd::pwdUpdate, this, &Profile::changePassword);
}

Profile::~Profile()
{
    delete ui;
    delete changePwdDialog;
}

void Profile::setUsername(const QString& username){
    ui->username->setText(username);
}

void Profile::setImagePic(const QPixmap &imagePic){
    ui->image_pic->setPixmap(imagePic.scaled(200, 200));
}

void Profile::on_changePassword_clicked()
{
    this->changePwdDialog->show();
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

    if (img.size() == 0) {
        // Don't send message
        return;
    } else if (img.size() > 2000000){
        QMessageBox::warning(this, "Warning", "The file's dimension is greater than 2MB!");
        return;
    }

    emit changeImage(pix);
}
