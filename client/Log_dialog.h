#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include "Editor.h"
#include "ListFiles.h"

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

    void displayFiles(const QList<QString> p_files);

signals:
    void authDataSubmitted(QString p_user, QString p_passw);
    void fileSelected(QString p_filename);

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();

private:
    Ui::Log_Dialog *ui;
    Editor *editor;
    ListFiles *lf;
};

#endif // LOG_DIALOG_H
