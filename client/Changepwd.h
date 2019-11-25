#ifndef CHANGEPWD_H
#define CHANGEPWD_H

#include <QDialog>

namespace Ui {
class ChangePwd;
}

class ChangePwd : public QDialog
{
    Q_OBJECT

public:
    explicit ChangePwd(QWidget *parent = nullptr);
    ~ChangePwd();

private:
    Ui::ChangePwd *ui;
};

#endif // CHANGEPWD_H
