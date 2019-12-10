#include "Editor.h"
#include "ui_editor.h"
#include "Creators.h"
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
#include <QPrinter>
#include <QLineEdit>

#include <QPushButton>
#include <QCheckBox>
#include <iostream>
#include <QMetaObject>



Editor::Editor(ClientController *p_controller, QWidget *parent, const QList<Account>& contributorsOnline, const QList<Account>& contributorsOffline, const Account* main_account) :
    QMainWindow(parent), controller(p_controller), ui(new Ui::Editor), handlingOperation(false), localOperation(false), changingFormat(false), alignFlag(false)
{
    ui->setupUi(this);
    ui->textEdit->setAcceptRichText(true);
    m_textEdit = ui->textEdit;

    setWindowTitle("PoliDox");
    QString fileName = controller->getFilename();
    ui->currentFile->setText(fileName);
    setWindowIcon(QIcon("://images/images/logo_small.png"));

    m_textDoc = new QTextDocument(m_textEdit);
    m_textEdit->setDocument(m_textDoc);    
    m_localCursor = new QTextCursor(m_textDoc);        
    m_textEdit->setTextCursor(*m_localCursor);

    m_showUriDialog = new ShowUriDialog(this);
    QString uri = controller->getUri();
    m_showUriDialog->setUri(uri);

    // Initialize graphic
    initContributorsLists();
    bootContributorsLists(contributorsOnline, contributorsOffline);
    initRichTextToolBar();

    int TLines = ui->textEdit->document()->blockCount();
    ui->statusbar->showMessage(QString("Line:1 Col:1 TotLines:%3").arg(TLines));

    // Connect signals
    connect(m_textDoc, &QTextDocument::contentsChange, [&](int position, int charsRemoved, int charsAdded) {
        // If text changes because of a remote modification we mustn't emit the signal again,
        // otherwise we fall in an endless loop
        if (!handlingOperation) {

            if (charsAdded > this->textSize() || alignFlag) {
                qDebug() << "Adjusting chars added and removed";
                charsAdded--;
                charsRemoved--;
                alignFlag = false;
            }

            // We call this asynchrounously, since cursor coordinates are not updated yet
            localOperation = true;
            QMetaObject::invokeMethod(this, "updateCursors", Qt::QueuedConnection);
            emit textChanged(position, charsRemoved, charsAdded);
        }
    });

    connect(m_textDoc, &QTextDocument::cursorPositionChanged, this, [&](){
        int line = ui->textEdit->textCursor().blockNumber()+1;
        int pos = ui->textEdit->textCursor().columnNumber()+1;
        int TLines = ui->textEdit->document()->blockCount();

        ui->statusbar->showMessage(QString("Line:%1 Col:%2 TotLines:%3").arg(line).arg(pos).arg(TLines));
    });

    connect(m_textEdit, &QTextEdit::cursorPositionChanged, this, [&](){
        int pos = m_textEdit->textCursor().position();
        resetActionToggle();
        if (localOperation || handlingOperation)
            localOperation = false;
        else
            emit cursorPositionChanged(pos);
    });


    profile = new Profile(this);
    profile->setImagePic(main_account->getImage());
    profile->setUsername(main_account->getName());

    QWidget* empty1 = new QWidget(this);
    empty1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    ui->toolBar_2->addWidget(empty1);

    QAction* account = new QAction("account", this);
    QPixmap acc("://images/images/account.png");
    QIcon accoutIcon(acc);
    account->setIcon(accoutIcon);
    ui->toolBar_2->addAction(account);

    QWidget* empty2 = new QWidget(this);

    empty2->setFixedSize(1,1);
    ui->toolBar_2->addWidget(empty2);

    connect(account,&QAction::triggered,this,&Editor::actionAccount_triggered);

    connect(profile, &Profile::changeImage, this, &Editor::ChangeImgEditor);

    connect(profile, &Profile::changePassword, this, &Editor::ChangePwdEditor);

}

Editor::~Editor()
{    
    // All other objects are destroyed with the widget tree
    delete ui;
}

void Editor::init(const QVector<Char>& p_text)
{
    handlingOperation = true;
    for (const Char& symbol : p_text) {
        addChar(symbol);
    }
    m_textEdit->show();

    updateCursors();
    handlingOperation = false;        
}

void Editor::bootContributorsLists(QList<Account> contributorsOnline, QList<Account> contributorsOffline){

    //fill offline list
    for(Account acc : contributorsOffline){
        assignRandomColor(acc.getSiteId());
        m_offlineUsers.append(acc);
        addOfflineUser(acc);
    }

    //fill online list
    for(Account acc : contributorsOnline) {

        assignRandomColor(acc.getSiteId());
        addClient(acc); // m_onlineUsers.add + addOnlineUser
    }
}

void Editor::initContributorsLists(){

    QPixmap online("://images/images/online.png");
    QIcon onlineIcon(online);
    ui->label->setPixmap(onlineIcon.pixmap(QSize(10,10)));

    QPixmap offline("://images/images/offline.png");
    QIcon offlineIcon(offline);
    ui->label_2->setPixmap(offlineIcon.pixmap(QSize(10,10)));

    int you=controller->getAccount().getSiteId();
    assignRandomColor(you);

    QColor color=QColor(assignedColor.value(you));

    QListWidgetItem* item = new QListWidgetItem("You", ui->onlineList);
    setItem(color,item);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);

    ui->onlineList->addItem(item);

    connect(ui->onlineList,&QListWidget::itemChanged,this,&Editor::highlightUser);
    connect(ui->offlineList,&QListWidget::itemChanged,this,&Editor::highlightUser);

}

void Editor::initRichTextToolBar(){

    QFontComboBox *font = new QFontComboBox(ui->textRichToolBar);
    QSpinBox *fontSize = new QSpinBox(ui->textRichToolBar);
    font->setObjectName("font");
    fontSize->setObjectName("font_size");

    m_textEdit->setFont(QFont(DEFAULT_FONT));
    font->setFont(m_textEdit->currentFont());
    m_textEdit->setFontPointSize(DEFAULT_SIZE);
    fontSize->setValue(DEFAULT_SIZE);

    this->ui->textRichToolBar->addWidget(font);
    this->ui->textRichToolBar->addWidget(fontSize);

    connect(font, &QFontComboBox::currentFontChanged, this, [&](const QFont& font){
        if (!changingFormat)
            onFontFamilyChanged(font);
    });

    connect(fontSize, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&](int i) {
        if (!changingFormat)
            onFontSizeChanged(i);
    });

    connect(m_textEdit, &QTextEdit::currentCharFormatChanged, this, &Editor::onCharFormatChanged);

    QAction* separator1 = this->ui->toolBar_2->actions().at(0);
    QAction* separator2 = this->ui->textRichToolBar->actions().at(0);
    delete separator1;
    delete separator2;

}

void Editor::setItem(QColor color,QListWidgetItem* item){

    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    color.setAlpha(80);
    item->setBackgroundColor(color);

}

void Editor::assignRandomColor(int siteID){

    QColor color;
    QList<QString> colors=assignedColor.values();

    while(1){
        color=QColor(rand()%220,rand()%220,rand()%220);
        auto HIT=std::find(colors.begin(),colors.end(),color.name());
        if(HIT==colors.end())
            break;

    }
    assignedColor.insert(siteID,color.name());
}

void Editor::highlightUser(QListWidgetItem *item) {

    int siteID = -1;
    QColor color;

    if ("You" == item->text()) {
        siteID = controller->getAccount().getSiteId();
        color=QColor(assignedColor.value(siteID));

    } else {

        // Search among the online users
        QList<User> valuesList = m_onlineUsers.values();
        auto onlineHIT = std::find_if(valuesList.begin(), valuesList.end(), [item](User user){
                return user.account.getName() == item->text();
        });

        if (onlineHIT == valuesList.end()) {
            // Search among the offline users
            auto offlineHIT = std::find_if(m_offlineUsers.begin(), m_offlineUsers.end(), [item](Account acc){
                return acc.getName() == item->text();
            });

            if (offlineHIT == m_offlineUsers.end()) {
                return;
            } else {
                siteID = offlineHIT->getSiteId();
                color=QColor(assignedColor.value(siteID));
            }

        } else {
            siteID = onlineHIT->account.getSiteId();
            color=QColor(assignedColor.value(siteID));
        }

    }

    bool checked = item->checkState() == Qt::Checked;

    highlightUserChars(siteID, color, checked);

}

void Editor::addOnlineUser(const Account& account){

    QListWidgetItem* _dItem=nullptr;
    QList<QListWidgetItem*> items = ui->offlineList->findItems(account.getName(), Qt::MatchFlag::MatchExactly);

    if(items.size()==0)
        qDebug() << "NEW USER FOR THIS FILE";
    else if(items.size()==1)
         _dItem=items.at(0);

    //DON'T SET THE PARENT HERE OTHERWISE ITEM CHANGHED WILL BE TRIGGERED WHEN BACKGROUND CHANGE
    QListWidgetItem* item= new QListWidgetItem(account.getName());

    QColor color(assignedColor.value(account.getSiteId()));
    setItem(color,item);


    if(items.size()>0){

        if(_dItem->checkState() == Qt::Checked)
            item->setCheckState(Qt::Checked);

        delete _dItem;
    }

    ui->onlineList->addItem(item);
}

void Editor::addOfflineUser(const Account& account)
{    
    QListWidgetItem* item = new QListWidgetItem(account.getName());
    QColor color(assignedColor.value(account.getSiteId()));
    setItem(color,item);
    ui->offlineList->addItem(item);
}

void Editor::addChar(const Char &p_char, QTextCursor& p_cursor)
{
    QTextCharFormat fmt;
    tStyle style = p_char.getStyle();

    if (style.font_family == "")
        fmt.setFontFamily(DEFAULT_FONT);
    else
        fmt.setFontFamily(style.font_family);

    fmt.setFontPointSize( p_char.getStyle().font_size );

    if (style.is_bold)
        fmt.setFontWeight(QFont::Bold);
    else
        fmt.setFontWeight(QFont::Normal);

    fmt.setFontItalic(style.is_italic);
    fmt.setFontUnderline(style.is_underline);

    p_cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

    p_cursor.insertText(QString(p_char.getValue()));

    QTextBlockFormat textBlockFormat = p_cursor.blockFormat();
    Qt::Alignment alignment = static_cast<Qt::Alignment>(style.alignment);
    textBlockFormat.setAlignment(alignment);//or another alignment
    p_cursor.mergeBlockFormat(textBlockFormat);

}

void Editor::addChar(const Char &p_char)
{
    QTextCharFormat fmt;
    tStyle style = p_char.getStyle();


    QTextCursor& localCursor=*m_localCursor;

    if (style.font_family == "")
        fmt.setFontFamily(DEFAULT_FONT);
    else
        fmt.setFontFamily(style.font_family);

    fmt.setFontPointSize( p_char.getStyle().font_size );

    if (style.is_bold)
        fmt.setFontWeight(QFont::Bold);
    else
        fmt.setFontWeight(QFont::Normal);

    fmt.setFontItalic(style.is_italic);
    fmt.setFontUnderline(style.is_underline);

    localCursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

    localCursor.insertText(QString(p_char.getValue()));

    QTextBlockFormat textBlockFormat = localCursor.blockFormat();
    Qt::Alignment alignment = static_cast<Qt::Alignment>(style.alignment);
    textBlockFormat.setAlignment(alignment);//or another alignment
    localCursor.mergeBlockFormat(textBlockFormat);

}

void Editor::removeClient(const Account& account){

    // 1. Update data structures
    delete  m_onlineUsers[account.getSiteId()].label;
    m_onlineUsers.remove(account.getSiteId());
    m_offlineUsers.append(account);

    // 2. Update view in editor
    QListWidgetItem* _dItem=nullptr;
    QList<QListWidgetItem*> items = ui->onlineList->findItems(account.getName(), Qt::MatchFlag::MatchExactly);

    _dItem=items.at(0); //there should be always one item in this list

    QListWidgetItem* item= new QListWidgetItem(account.getName()); //DON'T SET THE PARENT HERE OTHERWISE ITEM CHANGHED WILL BE TRIGGERED WHEN BACGROUND CHANGE

    QColor color(assignedColor.value(account.getSiteId()));
    setItem(color,item);

    if (_dItem->checkState() == Qt::Checked)
        item->setCheckState(Qt::Checked);

    delete _dItem;
    ui->offlineList->addItem(item);

}

QChar Editor::at(int pos)
{
    return m_textDoc->characterAt(pos);
}

int Editor::textSize()
{
    return m_textDoc->toPlainText().size();
}

void Editor::addClient(const Account& user)
{

    int siteId = user.getSiteId();

    if(m_offlineUsers.removeAll(user)==0) //new user for the file!
        assignRandomColor(siteId);

    // 1. Add user to the map of remote users
    QFont font("American Typewriter", 10, QFont::Bold);
    QLabel *remoteLabel = new QLabel(QString(user.getName()+"\n"), m_textEdit);
    QColor color(assignedColor.value(siteId));
    remoteLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    remoteLabel->setStyleSheet("color:"+color.name()+";background-color:transparent;border: 1px solid transparent;border-left-color:"+color.name()+";");
    remoteLabel->setFont(font);
    User newUser = { user, remoteLabel, QTextCursor(m_textDoc)};
    m_onlineUsers[siteId] = newUser;
    // 2. Draw the remote cursor at position 0
    QTextCursor& remoteCursor = m_onlineUsers[siteId].cursor;
    remoteCursor.setPosition(0);
    QRect curCoord = m_textEdit->cursorRect(remoteCursor);
    int height = curCoord.bottom()-curCoord.top();
    remoteLabel->resize(1000, height+5);

    /* update label dimension according to remote cursor position */
    QFont l_font=remoteLabel->font();
    QTextCharFormat fmt=remoteCursor.charFormat();
    int font_size=static_cast<int>(fmt.fontPointSize());
    if(font_size==0)
        font_size=DEFAULT_SIZE;
    QFont new_font(l_font.family(),static_cast<int>((font_size/2)+3),QFont::Bold);
    remoteLabel->setFont(new_font);

    remoteLabel->move(curCoord.left(), curCoord.top()-(remoteLabel->fontInfo().pointSize()/3));
    remoteLabel->setVisible(true);
    remoteLabel->raise();

    addOnlineUser(user);
}

void Editor::handleRemoteOperation(EditOp op, Char& symbol, int position, int siteId)
{
    handlingOperation = true;
    QTextCursor& remCursor = m_onlineUsers[siteId].cursor;

    remCursor.setPosition(position);
    if (op == INSERT_OP){
       addChar(symbol, remCursor);

       //This is needed to avoid that the character inserted copies the background of the previous character
       QColor color=userSelected(siteId);
       resetBackgroundColor(position, color);
    }
    else if (op == DELETE_OP)
       remCursor.deleteChar();

    updateCursors();
    handlingOperation = false;
}

void Editor::highlightUserChars(int p_siteId, QColor p_color, bool p_checked)
{
    QVector<int> userChars = controller->getUserChars(p_siteId);
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

    disconnect(this,&Editor::textChanged,controller,&ClientController::onTextChanged);

    QColor color;

    if(p_checked){
        color = p_color;
        color.setAlpha(80);
    }
    else
        color=QColor("transparent");

    /* remove selection when highlighting user chars*/
    QTextCursor c = m_textEdit->textCursor();
    c.clearSelection();
    m_textEdit->setTextCursor(c);

    for (auto it = map.begin(); it != map.end(); ++it){

        QTextCharFormat fmt;
        fmt.setBackground(color);
        QTextCursor cursor(m_textEdit->document());
        cursor.setPosition(it.key(), QTextCursor::MoveAnchor);
        cursor.setPosition(it.key()+it.value(), QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(fmt);
        m_textEdit->mergeCurrentCharFormat(fmt);
    }

    connect(this,&Editor::textChanged,controller,&ClientController::onTextChanged);

}

void Editor::resetBackgroundColor(int pos, const QColor& color){
    QTextCharFormat fmt;
    fmt.setBackground(color);
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
    bold = (fmt.fontWeight() == QFont::Bold);

    QFontComboBox *fontCombo=static_cast<QFontComboBox*>(ui->textRichToolBar->findChild<QFontComboBox*>("font"));
    QString font_fam=fontCombo->lineEdit()->text();

    QSpinBox *fontSize=static_cast<QSpinBox*>(ui->textRichToolBar->findChild<QSpinBox*>("font_size"));
    int font_size = fontSize->value();

    symbol.setStyle(font_fam, font_size, bold, fmt.fontItalic(),
                    fmt.fontUnderline(), static_cast<int>(m_textEdit->alignment()));
}


void Editor::resetActionToggle(){

    QTextCursor cursor=m_textEdit->textCursor();

    QTextCharFormat fmt=cursor.charFormat();

    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* center=this->ui->textRichToolBar->actions().at(5);
    QAction* right=this->ui->textRichToolBar->actions().at(6);
    QAction* justify=this->ui->textRichToolBar->actions().at(7);

    Qt::Alignment alignment=m_textEdit->alignment();

    if(alignment != Qt::AlignLeft && left->isChecked() == true)
        left->setChecked(false);
    else  if(alignment != Qt::AlignCenter && center->isChecked() == true)
        center->setChecked(false);
    else if(alignment != Qt::AlignRight && right->isChecked() == true)
        right->setChecked(false);
    else if(alignment != Qt::AlignJustify && justify->isChecked() == true)
        justify->setChecked(false);

    if(alignment == Qt::AlignLeft && left->isChecked() == false)
        left->setChecked(true);
    else  if(alignment == Qt::AlignCenter && center->isChecked() == false)
        center->setChecked(true);
    else if(alignment == Qt::AlignRight && right->isChecked() == false)
        right->setChecked(true);
    else if(alignment == Qt::AlignJustify && justify->isChecked() == false)
        justify->setChecked(true);



}

void Editor::updateCursors()
{
    for (auto it = m_onlineUsers.begin(); it != m_onlineUsers.end(); it++) {
        User& user = it.value();
        QRect remoteCoord = m_textEdit->cursorRect(user.cursor);
        int height = remoteCoord.bottom()-remoteCoord.top();
        user.label->resize(1000, height+5);

        /* update label dimension according to remote cursor position */
        QFont l_font=user.label->font();
        QTextCharFormat fmt=user.cursor.charFormat();
        int font_size=static_cast<int>(fmt.fontPointSize());
        if(font_size==0)
            font_size=DEFAULT_SIZE;
        QFont new_font(l_font.family(),static_cast<int>((font_size/2)+3),QFont::Bold);
        user.label->setFont(new_font);

        user.label->move(remoteCoord.left(), remoteCoord.top()-(user.label->fontInfo().pointSize()/3));
        user.label->setVisible(true);
    }
}

void Editor::moveCursor(int pos, int siteId)
{
    User& user = m_onlineUsers[siteId];
    user.cursor.setPosition(pos);
    QRect remoteCoord = m_textEdit->cursorRect(user.cursor);
    int height = remoteCoord.bottom()-remoteCoord.top();
    user.label->resize(1000, height+5);

    /* update label dimension according to remote cursor position */
    QFont l_font=user.label->font();
    QTextCharFormat fmt=user.cursor.charFormat();
    int font_size=static_cast<int>(fmt.fontPointSize());
    if(font_size==0)
        font_size=DEFAULT_SIZE;
    QFont new_font(l_font.family(),static_cast<int>((font_size/2)+3),QFont::Bold);
    user.label->setFont(new_font);

    user.label->move(remoteCoord.left(), remoteCoord.top()-(user.label->fontInfo().pointSize()/3));
    user.label->setVisible(true);
}

void Editor::onCharFormatChanged(const QTextCharFormat &f)
{
    changingFormat = true;
    ui->actionBold->setChecked(f.font().bold());
    ui->actionItalic->setChecked(f.font().italic());
    ui->actionUnderlined->setChecked(f.font().underline());
    QFontComboBox *font=static_cast<QFontComboBox*>(ui->textRichToolBar->findChild<QFontComboBox*>("font"));
    QSpinBox *fontSize=static_cast<QSpinBox*>(ui->textRichToolBar->findChild<QSpinBox*>("font_size"));
    font->setCurrentIndex(font->findText(QFontInfo(f.font()).family()));
    fontSize->setValue(f.font().pointSize());
    changingFormat = false;
}


void Editor::onFontSizeChanged(int i){

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt;

    fmt.setFontPointSize(i);    
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);
}

void Editor::onFontFamilyChanged(const QFont& font){

    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat fmt;

    fmt.setFontFamily(font.family());    
    cursor.mergeCharFormat(fmt);
    m_textEdit->mergeCurrentCharFormat(fmt);

}

QColor Editor::userSelected(int siteId){


    QList<QListWidgetItem*> items;

    if(controller->getAccount().getSiteId()==siteId)
        items = ui->onlineList->findItems("You", Qt::MatchFlag::MatchExactly);
    else{
        QString name=m_onlineUsers[siteId].account.getName();
        items = ui->onlineList->findItems(name, Qt::MatchFlag::MatchExactly);
    }

    if(items.at(0)->checkState() == Qt::Checked){
        QColor color(assignedColor[siteId]);
        color.setAlpha(80);
        return color;
    }
    else
        return QColor("transparent");

}

void Editor::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    on_actionQuit_triggered();
}

/* Handler di gestione per la creazione di un nuovo file */
void Editor::on_actionNew_triggered()
{
    this->hide();
    emit quit_editor_new_file();
}

/* Handler di gestione per il salvataggio ed esportazione del file in formato .PDF*/
void Editor::on_actionSave_as_triggered()
{

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Export PDF", QString(), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    doc.setHtml(m_textEdit->toHtml());
    doc.setPageSize(printer.pageRect().size()); // hide the page number
    doc.print(&printer);

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

void Editor::on_actionAlignLeft_triggered()
{
    QAction* center = ui->textRichToolBar->actions().at(5);
    QAction* right = ui->textRichToolBar->actions().at(6);
    QAction* justify = ui->textRichToolBar->actions().at(7);
    center->setChecked(false);
    right->setChecked(false);
    justify->setChecked(false);

    alignFlag=true;

    m_textEdit->setAlignment(Qt::AlignLeft);
}

void Editor::on_actionAlignCenter_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* right=this->ui->textRichToolBar->actions().at(6);
    QAction* justify = ui->textRichToolBar->actions().at(7);
    left->setChecked(false);
    right->setChecked(false);
    justify->setChecked(false);

    alignFlag=true;

    m_textEdit->setAlignment(Qt::AlignCenter);
}


void Editor::on_actionAlignRight_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* center=this->ui->textRichToolBar->actions().at(5);
    QAction* justify = ui->textRichToolBar->actions().at(7);
    left->setChecked(false);
    center->setChecked(false);
    justify->setChecked(false);

    alignFlag=true;

    m_textEdit->setAlignment(Qt::AlignRight);
}

void Editor::on_actionJustify_triggered()
{
    QAction* left=this->ui->textRichToolBar->actions().at(4);
    QAction* center=this->ui->textRichToolBar->actions().at(5);
    QAction* right=this->ui->textRichToolBar->actions().at(6);    
    left->setChecked(false);
    center->setChecked(false);
    right->setChecked(false);

    m_textEdit->setAlignment(Qt::AlignJustify);
}

/* Handler di gestione dell'apertura di un nuovo file */
void Editor::on_actionOpen_triggered()
{
     on_actionQuit_triggered();
}


void Editor::on_actionURI_triggered()
{    
    this->m_showUriDialog->show();
}

void Editor::actionAccount_triggered(bool checked)
{
    Q_UNUSED(checked)
    profile->show();

}

void Editor::setNewImage(const QPixmap& Pix){
    profile->setImagePic(Pix);
}

Profile *Editor::getProfilePtr(){
    return this->profile;
}

void Editor::on_actioncreators_triggered()
{
    Creators *creators = new Creators(this);
    creators->show();
}
