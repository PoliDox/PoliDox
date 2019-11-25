#ifndef CHANGEIMG_H
#define CHANGEIMG_H

#include <QDialog>

namespace Ui {
class ChangeImg;
}

class ChangeImg : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeImg(QWidget *parent = nullptr);
    ~ChangeImg();

private slots:

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ChangeImg *ui;
};

#endif // CHANGEIMG_H
