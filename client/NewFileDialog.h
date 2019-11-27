#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewFileDialog(QWidget *parent = nullptr);
    ~NewFileDialog();
    QString getNewFileName();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::NewFileDialog *ui;
    QString newFileName;

signals:
    void getFileName(QString newfilename);
};

#endif // NEWFILEDIALOG_H
