#include "Editor.h"
#include <QDebug>
#include <QChar>

Editor::Editor(QWidget *parent) : QMainWindow(parent), handlingRemoteOp(false)
{
    m_textEdit = new QTextEdit(this);
    setCentralWidget(m_textEdit);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_remoteCursor = new QTextCursor(m_textDoc);

    connect(m_textDoc, &QTextDocument::contentsChange, [&](int position, int charsRemoved, int charsAdded) {
        // If text changes because of a remote modification we mustn't emit the signal again,
        // otherwise we fall in an endless loop
        if (!handlingRemoteOp) {
           emit textChanged(position, charsRemoved, charsAdded);
        }
    });


}


QChar Editor::at(int pos)
{
    return m_textDoc->characterAt(pos);
}

void Editor::remoteInsert(int position, char ch)
{
    handlingRemoteOp = true;
    m_remoteCursor->clearSelection();
    m_remoteCursor->setPosition(position);
    m_remoteCursor->insertText(QString(ch));
    handlingRemoteOp = false;
}

void Editor::remoteDelete(int position)
{
    handlingRemoteOp = true;
    m_remoteCursor->clearSelection();
    m_remoteCursor->setPosition(position);
    m_remoteCursor->deleteChar();
    handlingRemoteOp = false;
}
