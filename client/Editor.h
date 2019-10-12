#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include "CrdtClient.h"


class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);

    QChar at(int pos);   

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);

public slots:
    void remoteInsert(int position, char ch);
    void remoteDelete(int position);
    void newFile();


private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void setCurrentFile(const QString &fileName);
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_remoteCursor;
    bool handlingRemoteOp;
    QString curFile;
};

#endif // EDITOR_H
