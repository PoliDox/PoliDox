#include "ShowUriDialog.h"
#include "ui_showuridialog.h"
#include <QTextFormat>
#include <QDebug>
#include <QIcon>

ShowUriDialog::ShowUriDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UriDialog)
{    
    ui->setupUi(this);
    ui->UriLabel->setAlignment(Qt::AlignHCenter);
    ui->UriLabel->setTextFormat(Qt::RichText);
    setWindowIcon(QIcon("://images/images/logo_small.png"));
    setWindowTitle("PoliDox");

}

ShowUriDialog::~ShowUriDialog()
{
    delete ui;
}

void ShowUriDialog::setUri(QString uri){
    QString htmlUri = "<span style=\" font-size:14pt; font-weight:600;\">" + uri + "</span>";
    ui->UriLabel->setText(htmlUri);
}
