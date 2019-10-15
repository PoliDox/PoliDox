#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include <QDialog>
#include "Editor.h"

namespace Ui {
class Log_Dialog;
}

class Log_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Log_Dialog(QWidget *parent = nullptr);
    ~Log_Dialog();
    void setEditor(Editor *);

private slots:
    void on_pushButton_login_clicked();

    void on_pushButton_register_clicked();

private:
    Ui::Log_Dialog *ui;
    Editor *editor;
};

#endif // LOG_DIALOG_H
