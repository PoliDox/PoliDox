#ifndef LOG_DIALOG_H
#define LOG_DIALOG_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QListWidgetItem>
#include <QPixmap>
#include "Editor.h"
#include "NewFileDialog.h"
#include "InsertUriDialog.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();
    void closeEvent(QCloseEvent *event) override;

    void setEditor(Editor *);

    void displayFiles(const QList<QString>& p_files);
    void registrationOk();
    void onClickedFile(QListWidgetItem *item);

signals:
    void authDataSubmitted(QString& p_user, QString& p_passw);
    void signupDataSubmitted(QString p_user, QString p_passw, QPixmap p_pic);
    void fileSelected(const QString& p_filename);
    void newFileSelected(QString& p_filename);
    void uriSelected(QString& p_uri);
    void closed();

private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
    void sendRegistrationData();
    void upload_clicked();

private:
    Ui::LoginWindow *ui = nullptr;
    NewFileDialog *nfd = nullptr;
    InsertUriDialog *uriDialog = nullptr;

    void createRegistrationForm();
    void cleanRegistrationForm();
    void showLoginForm();
    void hideLoginForm();
};

#endif // LOG_DIALOG_H
