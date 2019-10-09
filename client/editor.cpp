#include "Editor.h"
#include <QDebug>
#include <QChar>

Editor::Editor(QWidget *parent) : QMainWindow(parent)
{
    m_textEdit = new QTextEdit(this);
    setCentralWidget(m_textEdit);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_remoteCursor = new QTextCursor(m_textDoc);

    connect(m_textDoc, &QTextDocument::contentsChange, this, &Editor::textChanged);


}

QChar Editor::at(int pos)
{
    return m_textDoc->characterAt(pos);
}
