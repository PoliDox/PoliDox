#ifndef INSERTURIDIALOG_H
#define INSERTURIDIALOG_H

#include <QDialog>

namespace Ui {
class InsertUriDialog;
}

class InsertUriDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertUriDialog(QWidget *parent = nullptr);
    ~InsertUriDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InsertUriDialog *ui;
    QString uriName;

signals:
    void getUriName(QString& newfilename);
};


#endif // INSERTURIDIALOG_H
