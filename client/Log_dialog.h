#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QListWidgetItem>
#include "Editor.h"
#include "ListFiles.h"
#include "NewFileDialog.h"
#include "InsertUriDialog.h"

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
    void registrationOk();

signals:
    void authDataSubmitted(QString p_user, QString p_passw);
    void signupDataSubmitted(QString p_user, QString p_passw);
    void fileSelected(QString p_filename);
    void newFileSelected(QString p_filename);
    void uriSelected(QString p_uri);

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void onClickedFile(QListWidgetItem *item);
    void sendRegistrationData();

private:
    Ui::Log_Dialog *ui;
    Editor *editor;
    ListFiles *lf;
    NewFileDialog *nfd;
    InsertUriDialog *uriDialog;

    void createRegistrationForm();
    void cleanRegistrationForm();
    void showLoginForm();
    void hideLoginForm();
};

#endif // LOG_DIALOG_H
