#ifndef LISTFILES_H
#define LISTFILES_H

#include <QWidget>
#include "NewFileDialog.h"

namespace Ui {
class ListFiles;
}

class ListFiles : public QWidget
{
    Q_OBJECT

public:
    explicit ListFiles(QWidget *parent = nullptr);
    ~ListFiles();
    void displayFiles(const QList<QString> p_files);

private slots:
    void on_NewFile_clicked();

    void on_Open_clicked();

    void on_Delete_clicked();

    void on_GenerateLink_clicked();

private:
    Ui::ListFiles *ui;
    NewFileDialog *nfd;
    void createNewEditor_fromNew(QString filename);

signals:
    void makeNewEditor_fromNew(QString filename);
};

#endif // LISTFILES_H
