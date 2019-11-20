#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>

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

private:
    Ui::Profile *ui;
};

#endif // PROFILE_H
