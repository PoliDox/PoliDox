#include "Editor.h"
#include "ui_editor.h"
#include <QDebug>
#include <QChar>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QSettings>
#include <QStatusBar>
#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

Editor::Editor(QWidget *parent) : QMainWindow(parent), handlingRemoteOp(false), ui(new Ui::Editor)

{
    ui->setupUi(this);
    m_textEdit = ui->textEdit;
    setCentralWidget(m_textEdit);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_localCursor = new QTextCursor(m_textDoc);
    setWindowTitle("PoliDox");

    connect(m_textDoc, &QTextDocument::contentsChange, [&](int position, int charsRemoved, int charsAdded) {
        // If text changes because of a remote modification we mustn't emit the signal again,
        // otherwise we fall in an endless loop
        if (!handlingRemoteOp) {
           emit textChanged(position, charsRemoved, charsAdded);
        }
    });

}

Editor::~Editor()
{
    delete ui;
}


QChar Editor::at(int pos)
{
    return m_textDoc->characterAt(pos);
}

void Editor::addClient(int siteId)
{
    m_remoteCursors[siteId] = new QLabel(QString("|"), m_textEdit);
}

void Editor::remoteInsert(int position, char ch)
{
    handlingRemoteOp = true;
    m_localCursor->clearSelection();
    m_localCursor->setPosition(position);
    m_localCursor->insertText(QString(ch));
    handlingRemoteOp = false;
    QLabel *qLbl = new QLabel(QString("|"), m_textEdit);
    m_textEdit->moveCursor(QTextCursor::Start);
    const QRect qRect = m_textEdit->cursorRect();
    //qDebug() << "Rect:" << qRect;
    //QLabel *qLbl = new QLabel(QString("Ciao a tutti!!"), m_textEdit);
    qLbl->setVisible(true);
    //qLbl->show();
    qLbl->move(qRect.left(), qRect.top());
}

void Editor::remoteDelete(int position)
{
    handlingRemoteOp = true;
    m_localCursor->clearSelection();
    m_localCursor->setPosition(position);
    m_localCursor->deleteChar();
    handlingRemoteOp = false;
}


/* Handler di gestione per la creazione di un nuovo file */
void Editor::on_actionNew_triggered()
{
    /* TODO: implementare la creazione di un nuovo file qui */
}

/* Handler di gestione dell'apertura di un nnuovo file */
void Editor::on_actionOpen_triggered()
{
    /* TODO: implementare l'apertura di un file già esistente qui */

    //PROCEDURA PER L'APERTURA DI UN FILE IN LOCALE, NON E' IL NOSTRO CASO. NOI LO VOGLIAMO PRENDERE DAL SERVER.
    //L'ho scritta perché le funzioni possono tornarci utili. Lasciamola commentata al momento
    /*
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    curFile = fileName;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    m_textEdit->setText(text);
    file.close();
    */
}

/* Handler di gestione per il salvataggio ed esportazione del file */
void Editor::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = m_textEdit->toPlainText();
    out << text;
    file.close();

}

void Editor::on_actionQuit_triggered()
{
    QApplication::quit();
}

void Editor::on_actionCopy_triggered()
{
    m_textEdit->copy();
}


void Editor::on_actionPaste_triggered()
{
    m_textEdit->paste();
}


void Editor::on_actionCut_triggered()
{
    m_textEdit->cut();
}

void Editor::on_actionUndo_triggered()
{
    m_textEdit->undo();
}

void Editor::on_actionRedo_triggered()
{
    m_textEdit->redo();
}

