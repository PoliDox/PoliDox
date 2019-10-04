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

signals:

public slots:

private:
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    CRDTclient m_crdt;

    QTextCursor *m_remoteCursor;
};

#endif // EDITOR_H
