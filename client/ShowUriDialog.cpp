#include "ShowUriDialog.h"
#include "ui_showuridialog.h"
#include <QTextFormat>
#include <QDebug>

ShowUriDialog::ShowUriDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UriDialog)
{    
    ui->setupUi(this);
    ui->UriLabel->setAlignment(Qt::AlignHCenter);
    ui->UriLabel->setTextFormat(Qt::RichText);
}

ShowUriDialog::~ShowUriDialog()
{
    delete ui;
}

void ShowUriDialog::setUri(QString uri){
    QString htmlUri = "<span style=\" font-size:14pt; font-weight:600;\">" + uri + "</span>";
    //QString htmlUri = uri;
    //qDebug() << "htmlUri: " << htmlUri;
    ui->UriLabel->setText(htmlUri);
}
