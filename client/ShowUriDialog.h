#ifndef URIDIALOG_H
#define URIDIALOG_H

#include <QDialog>

namespace Ui {
class UriDialog;
}

class ShowUriDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShowUriDialog(QWidget *parent = nullptr);
    ~ShowUriDialog();

    void setUri(QString uri);

private:
    Ui::UriDialog *ui;
};

#endif // URIDIALOG_H
