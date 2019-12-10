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

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

signals:
<<<<<<< Updated upstream
    void pwdUpdate(QString& pwd);
=======
    void PwdUpdate(QString& pwd);
>>>>>>> Stashed changes

private:
    Ui::ChangePwd *ui;
};

#endif // CHANGEPWD_H
