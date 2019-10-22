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
#include <QSpinBox>
#include <QFontComboBox>
#include <iostream>

Editor::Editor(QWidget *parent) : QMainWindow(parent), handlingRemoteOp(false), ui(new Ui::Editor)

{
    ui->setupUi(this);
    this->ui->textEdit->setAcceptRichText(true);
    m_textEdit = ui->textEdit;
    this->ui->textEdit->setStyleSheet( "background-color:white");
    this->ui->toolBar_2->setStyleSheet( "background-color:transparent");
    setCentralWidget(m_textEdit);
    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_localCursor = new QTextCursor(m_textDoc);
    m_textEdit->setTextCursor(*m_localCursor);
    setWindowTitle("PoliDox");

    connect(m_textDoc, &QTextDocument::contentsChange, [&](int position, int charsRemoved, int charsAdded) {
        // If text changes because of a remote modification we mustn't emit the signal again,
        // otherwise we fall in an endless loop
        if (!handlingRemoteOp) {
           emit textChanged(position, charsRemoved, charsAdded);
        }
    });


    /* SETTING TOOLBAR */
    QFontComboBox* font=new QFontComboBox(this->ui->textRichToolBar);
    m_textEdit->setFont(font->currentFont());
    this->ui->textRichToolBar->addWidget(font);
    connect(font,&QFontComboBox::currentFontChanged,this,&Editor::fontFamilyChanged);
    m_textEdit->setFontPointSize(15);
    QSpinBox* spinBox=new QSpinBox(this->ui->textRichToolBar);
    spinBox->setValue(m_textEdit->fontPointSize());
    std::cout << spinBox->value() << std::endl;
    QAction* spinAction=this->ui->textRichToolBar->addWidget(spinBox);
    connect(spinBox,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&Editor::fontSizeChanged);

    int TLines = ui->textEdit->document()->blockCount();
    ui->statusbar->showMessage(QString("Col:1 Line:1 TotLines:%3").arg(TLines));

    connect(m_textDoc, &QTextDocument::cursorPositionChanged, this, [&](){
        int line = ui->textEdit->textCursor().blockNumber()+1;
        int pos = ui->textEdit->textCursor().columnNumber()+1;
        int TLines = ui->textEdit->document()->blockCount();

        ui->statusbar->showMessage(QString("Col:%1 Line:%2 TotLines:%3").arg(line).arg(pos).arg(TLines));
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

void Editor::addClient(Account& user)
{
    m_users.append(user);

    int siteId = user.getSiteId();
    m_remoteCursors[siteId] = new QLabel(QString("|"), m_textEdit);
    // TODO: Uncomment these lines when siteId is supported
    m_remoteCursors[siteId]->setVisible(true);
    QRect curCoord = m_textEdit->cursorRect(*m_localCursor);
    m_remoteCursors[siteId]->move(curCoord.left(), curCoord.top());
}

void Editor::remoteInsert(int siteId, int position, char ch)
{
    handlingRemoteOp = true;
    int origPos = m_localCursor->position();
    m_localCursor->clearSelection();
    m_localCursor->setPosition(position);    
    m_localCursor->insertText(QString(ch));
    handlingRemoteOp = false;
    // TODO: Uncomment these lines when siteId is supported
    //m_localCursor->setPosition(position+1);
    //QRect remoteCoord = m_textEdit->cursorRect(*m_localCursor);
    //m_remoteCursors[siteId]->move(remoteCoord.left(), remoteCoord.top());
    m_localCursor->setPosition(origPos);
}

void Editor::remoteDelete(int siteId, int position)
{
    // TODO: implement it (see remoteInsert)
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


void Editor::on_actionBold_triggered()
{

    QTextCharFormat fmt;
    fmt.setFontWeight(this->ui->textRichToolBar->actions().at(1)->isChecked() ? QFont::Bold : QFont::Normal);

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);


}

void Editor::on_actionItalic_triggered()
{

    QTextCharFormat fmt;
    fmt.setFontItalic(this->ui->textRichToolBar->actions().at(2)->isChecked());

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

}

void Editor::on_actionUnderlined_triggered()
{

    QTextCharFormat fmt;
    fmt.setFontUnderline(this->ui->textRichToolBar->actions().at(3)->isChecked());

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

}

void Editor::fontSizeChanged(int i){

    m_textEdit->setFontPointSize(i);

}

void Editor::fontFamilyChanged(const QFont& font){

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt;

    fmt.setFont(font);
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

}
