#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>

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
};

#endif // EDITOR_H
