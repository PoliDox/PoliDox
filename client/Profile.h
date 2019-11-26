#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>
#include <Changepwd.h>

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public:
    Profile(QWidget *parent = nullptr);
    ~Profile();
    void setUsername(const QString& username);
    void setImagePic(const QPixmap& imagePic);

private slots:
    void on_changePassword_clicked();

    void on_changeImage_clicked();

private:
    Ui::Profile *ui;
    ChangePwd *changePwdDialog;

signals:
    void ChangeImage(QPixmap);

};

#endif // PROFILE_H
