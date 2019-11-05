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
#include <QPushButton>
#include <QCheckBox>
#include <iostream>

void setItem(QColor color,QListWidgetItem* item){

    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    color.setAlpha(80);
    item->setBackgroundColor(color);

}

Editor::Editor(ClientController *p_controller, QWidget *parent, QString fileName, const QList<Account>& contributorsOnline, const QList<Account>& contributorsOffline) :
    QMainWindow(parent), controller(p_controller), handlingOperation(false), ui(new Ui::Editor)
{
    ui->setupUi(this);
    ui->textEdit->setAcceptRichText(true);
    m_textEdit = ui->textEdit;

    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);
    m_localCursor = new QTextCursor(m_textDoc);
    m_textEdit->setTextCursor(*m_localCursor);

    setWindowTitle("PoliDox");
    ui->currentFile->setText(fileName);

    initUserList();
    bootContributorsLists(contributorsOnline, contributorsOffline);

    connect(m_textDoc, &QTextDocument::contentsChange, [&](int position, int charsRemoved, int charsAdded) {
        // If text changes because of a remote modification we mustn't emit the signal again,
        // otherwise we fall in an endless loop
        if (!handlingOperation) {
           //qDebug() << "QTextDocument::contentsChange";
           updateCursors();
           emit textChanged(position, charsRemoved, charsAdded);
        }
    });


    initRichTextToolBar();


    int TLines = ui->textEdit->document()->blockCount();
    ui->statusbar->showMessage(QString("Line:1 Col:1 TotLines:%3").arg(TLines));

    connect(m_textDoc, &QTextDocument::cursorPositionChanged, this, [&](){
        int line = ui->textEdit->textCursor().blockNumber()+1;
        int pos = ui->textEdit->textCursor().columnNumber()+1;
        int TLines = ui->textEdit->document()->blockCount();

        ui->statusbar->showMessage(QString("Line:%1 Col:%2 TotLines:%3").arg(line).arg(pos).arg(TLines));
    });

    connect(controller,&ClientController::newUserOnline,this,&Editor::addOnlineUser);
    connect(controller,&ClientController::userOffline,this,&Editor::addOfflineUser);


}

/*bool Editor::eventFilter(QObject *target, QEvent *event){

    if(event->type()==QEvent::MouseButtonPress){
        QMouseEvent* mouse_event=static_cast<QMouseEvent*>(event);
        if(mouse_event->button()==Qt::LeftButton)
            std::cout << "Mouse left event captured!" << std::endl;
        else
            std::cout << "Mouse right event captured!" << std::endl;

        return true;
    }

    QMainWindow::eventFilter(target,event);

}*/


void Editor::bootContributorsLists(QList<Account> contributorsOnline, QList<Account> contributorsOffline){
    //fill offline list
    //No addOfflineUser because no online users to move at boot time
    for(Account acc : contributorsOffline){

        QListWidgetItem* item= new QListWidgetItem(acc.getName());
        setItem(acc.getColor(),item);
        ui->offlineList->addItem(item);
    }

    //fill online list
    for(Account acc : contributorsOnline)
        addOnlineUser(acc);
}

void Editor::initUserList(){

    QPixmap online("./online.png");
    QIcon onlineIcon(online);
    ui->label->setPixmap(onlineIcon.pixmap(QSize(10,10)));

    QPixmap offline("./offline.png");
    QIcon offlineIcon(offline);
    ui->label_2->setPixmap(offlineIcon.pixmap(QSize(10,10)));

    QListWidgetItem* item= new QListWidgetItem("You",ui->onlineList);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);

    ui->onlineList->addItem(item);

    connect(ui->onlineList,&QListWidget::itemChanged,this,&Editor::highLightUser);
    connect(ui->offlineList,&QListWidget::itemChanged,this,&Editor::highLightUser);

}

void Editor::highLightUser(QListWidgetItem * item){

    int siteID=-1;

    QList<User> valuesList = m_users.values(); // get a list of all the values

    auto userHIT=std::find_if(valuesList.begin(),valuesList.end(),[item](User user){

            if(user.account.getName()==item->text().toUtf8().constData())
                return true;
            else
                return false;
    });

    if(userHIT!=valuesList.end())
        siteID=userHIT->account.getSiteId();
    else
        std::cout << "PANIC! USER ONLINE NOT FOUND" << std::endl;

    QVector<int> userChars = controller->getUserChars(siteID);
    QMap<int,int> map;

    int start=0,
        lenght=0;

    if(userChars.size()>0){

        for(int i=1;i<=userChars.size();i++){

            if(lenght==0)
                start=userChars[i-1];

            lenght++;

            if(i==userChars.size()){
                 map.insert(start,lenght);
                 break;
            }

            if(userChars[i]-userChars[i-1]>1){

                map.insert(start,lenght);
                lenght=0;

            }
        }
    }

    // TODO: could be dangerous, use handlingOperation instead?
    disconnect(this,&Editor::textChanged,controller,&ClientController::onTextChanged);

    QColor color;

    if(item->checkState() == Qt::Checked){
        color=QColor(m_users[siteID].account.getColor());
        color.setAlpha(80);
    }
    else
        color=QColor("transparent");

    for (auto it = map.begin(); it != map.end(); ++it){

        QTextCharFormat fmt;
        fmt.setBackground(color);
        QTextCursor cursor(m_textEdit->document());
        cursor.setPosition(it.key(), QTextCursor::MoveAnchor);
        cursor.setPosition(it.key()+it.value(), QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(fmt);
        m_textEdit->mergeCurrentCharFormat(fmt);
    }

    // TODO: could be dangerous, use handlingOperation instead?
    connect(this,&Editor::textChanged,controller,&ClientController::onTextChanged);


}

void Editor::addOnlineUser(Account account){

    QListWidgetItem* _dItem;
    QList<QListWidgetItem*> items = ui->offlineList->findItems(account.getName(), Qt::MatchFlag::MatchExactly);

    if(items.size()==0)
        std::cout << "NEW USER FOR THIS FILE"<< std::endl;
    else if(items.size()>1)
        std::cout << "PANIC! MORE USER WITH SAME USERNAME"<< std::endl;
    else if(items.size()==1)
         _dItem=items.at(0); //there should be always one item in this list

    QListWidgetItem* item= new QListWidgetItem(account.getName()); //DON'T SET THE PARENT HERE OTHERWISE ITEM CHANGHED WILL BE TRIGGERED WHEN BACGROUND CHANGE

    setItem(account.getColor(),item);

    if(items.size()>0){

        if(_dItem->checkState() == Qt::Checked)
            item->setCheckState(Qt::Checked);

        delete _dItem;
    }


    ui->onlineList->addItem(item);

}

void Editor::addOfflineUser(Account account){

    QListWidgetItem* _dItem;
    QList<QListWidgetItem*> items = ui->onlineList->findItems(account.getName(), Qt::MatchFlag::MatchExactly);

    if (items.size()==0)
        std::cout << "PANIC! USER NOT FOUND"<< std::endl;
    else if (items.size()>1)
        std::cout << "PANIC! MORE USER WITH SAME USERNAME"<< std::endl;
    else if (items.size()==1)
         _dItem=items.at(0); //there should be always one item in this list

    QListWidgetItem* item= new QListWidgetItem(account.getName()); //DON'T SET THE PARENT HERE OTHERWISE ITEM CHANGHED WILL BE TRIGGERED WHEN BACGROUND CHANGE

    setItem(account.getColor(),item);

    if (_dItem->checkState() == Qt::Checked)
        item->setCheckState(Qt::Checked);

    delete _dItem;
    ui->offlineList->addItem(item);

}

void Editor::initRichTextToolBar(){


    QFontComboBox* font=new QFontComboBox(this->ui->textRichToolBar);
    QSpinBox* spinBox=new QSpinBox(this->ui->textRichToolBar);

    m_textEdit->setFont(font->currentFont());
    m_textEdit->setFontPointSize(20);
    spinBox->setValue(m_textEdit->fontPointSize());

    this->ui->textRichToolBar->addWidget(font);
    this->ui->textRichToolBar->addWidget(spinBox);

    connect(font,&QFontComboBox::currentFontChanged,this,&Editor::fontFamilyChanged);
    connect(spinBox,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),this,&Editor::fontSizeChanged);

    QAction* separator1=this->ui->toolBar_2->actions().at(0);
    QAction* separator2=this->ui->textRichToolBar->actions().at(0);
    delete separator1;
    delete separator2;

}


Editor::~Editor()
{
    delete ui;
}

void Editor::init(const QString &p_text)
{
    handlingOperation = true;
    m_textEdit->setPlainText(p_text);
    m_textEdit->show();
    handlingOperation = false;
}


QChar Editor::at(int pos)
{
    return m_textDoc->characterAt(pos);
}

void Editor::addClient(const Account& user)
{   
    // Add user to the map of remote users
    int siteId = user.getSiteId();
    QLabel *remoteLabel = new QLabel(QString(user.getName()+"\n|"), m_textEdit);
    remoteLabel->setStyleSheet("color:"+user.getColor().name()+";background-color:transparent;font-family:American Typewriter;font-weight:bold");
    remoteLabel->lower();
    User newUser = { user, remoteLabel, QTextCursor(m_textDoc)};
    m_users[siteId] = newUser;    

    // Draw the remote cursor at position 0
    QTextCursor& remoteCursor = m_users[siteId].cursor;
    remoteCursor.setPosition(0);
    QRect curCoord = m_textEdit->cursorRect(remoteCursor);
    remoteLabel->move(curCoord.left()-2, curCoord.top()-7);
    remoteLabel->setVisible(true);
    //m_textEdit->raise();
}

void Editor::handleRemoteOperation(EditOp op, int siteId, int position, char ch)
{
    /*
    qDebug() << "Cursors positions: ";
    for (auto it = m_users.begin(); it != m_users.end(); it++) {
        int tmp = it.key();
        User& tmpp = it.value();
        qDebug() << tmp << ": " << tmpp.cursor.position();
    }
    qDebug() << "local: " << m_localCursor->position();
    */

    handlingOperation = true;
    QTextCursor& remCursor = m_users[siteId].cursor;
    /* SETTAGGIO STILE */
    remCursor.setPosition(position);
    if (op == INSERT_OP)
        remCursor.insertText(QString(ch));
    else if (op == DELETE_OP)
       remCursor.deleteChar();

    updateCursors();
    handlingOperation = false;
}

void Editor::updateCursors()
{
    for (auto it = m_users.begin(); it != m_users.end(); it++) {

        User& user = it.value();
        QRect remoteCoord = m_textEdit->cursorRect(user.cursor);
        user.label->move(remoteCoord.left()-2, remoteCoord.top()-7);
        user.label->setVisible(true);
    }
}

void Editor::resetBackgroundColor(int pos){
    QTextCharFormat fmt;
    fmt.setBackground(QColor("transparent"));
    QTextCursor cursor(m_textDoc);
    cursor.setPosition(pos, QTextCursor::MoveAnchor);
    cursor.setPosition(pos+1, QTextCursor::KeepAnchor);
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void Editor::setCharacterStyle(int index, Char &symbol){

    bool bold;

    QTextCursor cursor(m_textDoc);
    cursor.setPosition(index); /* se testo abc il cursore a pos=1 indica a, pos=2 indica b */

    QTextCharFormat fmt=cursor.charFormat();

    if(fmt.fontWeight()==50)
        bold=false;
    else
        bold=true;

    symbol.setStyle(fmt.fontFamily(),fmt.fontPointSize(),bold,fmt.fontItalic(),fmt.fontUnderline(),m_textEdit->alignment());
}

void Editor::resetActionToggle(int pos,bool selection){

    QTextCursor cursor(m_textDoc);
    cursor.setPosition(pos); /* se testo abc il cursore a pos=1 indica a, pos=2 indica b */

    QTextCharFormat fmt=cursor.charFormat();

    QAction* boldAction=this->ui->textRichToolBar->actions().at(0);
    QAction* italicAction=this->ui->textRichToolBar->actions().at(1);
    QAction* underlineAction=this->ui->textRichToolBar->actions().at(2);

    if(fmt.fontWeight()==50 && boldAction->isChecked() && !selection)
        boldAction->setChecked(false);
    else if(fmt.fontWeight()==75)
        boldAction->setChecked(true);

    if(!fmt.fontItalic() && italicAction->isChecked() && !selection)
        italicAction->setChecked(false);
    else if(fmt.fontItalic())
            italicAction->setChecked(true);

    if(!fmt.fontUnderline() && underlineAction->isChecked() && !selection)
        underlineAction->setChecked(false);
    else if(fmt.fontUnderline())
        underlineAction->setChecked(true);

}

void Editor::closeEvent(QCloseEvent *event)
{
    on_actionQuit_triggered();
}

/* Handler di gestione per la creazione di un nuovo file */
void Editor::on_actionNew_triggered()
{
    /* TODO: implementare la creazione di un nuovo file qui */
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
    this->hide();
    emit quit_editor();    
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
    fmt.setFontWeight(this->ui->textRichToolBar->actions().at(0)->isChecked() ? QFont::Bold : QFont::Normal);

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);


}

void Editor::on_actionItalic_triggered()
{

    QTextCharFormat fmt;
    fmt.setFontItalic(this->ui->textRichToolBar->actions().at(1)->isChecked());

    QTextCursor cursor = m_textEdit->textCursor();

    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

}

void Editor::on_actionUnderlined_triggered()
{

    QTextCharFormat fmt;
    fmt.setFontUnderline(this->ui->textRichToolBar->actions().at(2)->isChecked());

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

void Editor::on_actionLeftAllignmet_triggered()
{
    QAction* center=this->ui->textRichToolBar->actions().at(5);
    QAction* right=this->ui->textRichToolBar->actions().at(6);

    if(center->isChecked())
        center->setChecked(false);
    else if(right->isChecked())
        right->setChecked(false);

    m_textEdit->setAlignment(Qt::AlignLeft);
}

void Editor::on_actionAlignCenter_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* right=this->ui->textRichToolBar->actions().at(6);

    if(left->isChecked())
        left->setChecked(false);
    else if(right->isChecked())
        right->setChecked(false);

    m_textEdit->setAlignment(Qt::AlignCenter);
}


void Editor::on_actionAlignRight_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* center=this->ui->textRichToolBar->actions().at(5);

    if(left->isChecked())
        left->setChecked(false);
    else if(center->isChecked())
        center->setChecked(false);

    m_textEdit->setAlignment(Qt::AlignRight);
}

void Editor::on_actionJustify_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* center=this->ui->textRichToolBar->actions().at(5);
    QAction* right=this->ui->textRichToolBar->actions().at(6);

    if(left->isChecked())
        left->setChecked(false);
    else if(center->isChecked())
        center->setChecked(false);
    else if(right->isChecked())
        right->setChecked(false);

    m_textEdit->setAlignment(Qt::AlignJustify);

}



/* void Editor::highlightUserChars(int p_siteId)
{

    QVector<int> userChars = controller->getUserChars(p_siteId);
    QTextCursor tmpCursor(m_textDoc);
    QColor color = m_users[p_siteId].account.getColor();
    QTextCharFormat format;
    format.setBackground(color.lighter(160));
    for (int charPos : userChars) {
        tmpCursor.setPosition(charPos);
        tmpCursor.select(QTextCursor::WordUnderCursor);
        tmpCursor.mergeCharFormat(format);
        // MaBorghe perchè se una parola è lunga 20lettere la evidenzi 20 volte ? =(
    }
}
*/

/* Handler di gestione dell'apertura di un nuovo file */
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
