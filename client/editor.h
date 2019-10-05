#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include "CRDTclient.h"


class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);

    QChar at(int pos);

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);

public slots:

private:
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_remoteCursor;
};

#endif // EDITOR_H
