#include "editor.h"
#include <QDebug>

Editor::Editor(QWidget *parent) : QMainWindow(parent)
{
    m_textEdit = new QTextEdit(this);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    connect(m_textDoc, &QTextDocument::contentsChange, [=] (int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            qDebug() << "Added" << charsAdded << "chars at position" << position;
        } else {
            qDebug() << "Removed" << charsRemoved << "chars at position" << position;
        }
    });

    setCentralWidget(m_textEdit);
}
