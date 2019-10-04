#include "editor.h"
#include <QDebug>
#include <QChar>

Editor::Editor(QWidget *parent) : QMainWindow(parent)
{
    m_textEdit = new QTextEdit(this);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_remoteCursor = new QTextCursor(m_textDoc);
    connect(m_textDoc, &QTextDocument::contentsChange, [=] (int position, int charsRemoved, int charsAdded) {
        if (charsAdded) {
            //qDebug() << "Added" << charsAdded << "chars at position" << position;
            QChar car =  m_textDoc->characterAt(position);
            m_crdt.localInsert(position, car.toLatin1());
        } else {            
            //qDebug() << "Removed" << charsRemoved << "chars at position" << position;
            m_crdt.localDelete(position);
        }
    });

    setCentralWidget(m_textEdit);
}