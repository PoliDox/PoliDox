#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include <QMap>
#include <QLabel>
#include <QListWidgetItem>
#include "Account.h"
#include "Styler.h"
#include "ClientController.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Editor; }
QT_END_NAMESPACE

class ClientController;

struct User
{
    Account account;
    QLabel *label;
    QTextCursor cursor;
    // TODO: Add colors here (or maybe we should hard-code them in a vector?)
};

enum EditOp
{
    INSERT_OP,
    DELETE_OP
};

class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor(ClientController *p_controller, QWidget *parent = nullptr, QString fileName = "", QList<Account> contributorsOnline = {}, QList<Account> contributorsOffline = {});
    ~Editor() override;

    void init(const QString &p_text);
    QChar at(int pos);
    void addClient(const Account& user);
    void handleRemoteOperation(EditOp op, int siteId, int position, char ch = 0);
    void resetBackgroundColor(int pos);
    void setCharacterStyle(int index,Char& symbol);
    void resetActionToggle(int pos,bool selection);
    void closeEvent(QCloseEvent *event) override;

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);
    void quit_editor();


private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_as_triggered();
    void on_actionCopy_triggered();
    void on_actionQuit_triggered();
    void on_actionPaste_triggered();
    void on_actionCut_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionBold_triggered();
    void on_actionItalic_triggered();
    void on_actionUnderlined_triggered();

    void fontSizeChanged(int i);
    void fontFamilyChanged(const QFont& font);

    void on_actionLeftAllignmet_triggered();
    void on_actionAlignCenter_triggered();
    void on_actionAlignRight_triggered();
    void on_actionJustify_triggered();

    void addOnlineUser(Account account);
    void addOfflineUser(Account account);

    void highLightUser(QListWidgetItem * item);

private:
    void createActions();
    void createStatusBar();
    void initUserList();
    void initRichTextToolBar();
    void updateCursors();
    void highlightUserChars(int p_siteId);
    void bootContributorsLists(QList<Account> contributorsOnline, QList<Account> contributorsoffline);

    QString fileName;
    ClientController *controller;
    bool handlingOperation;
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_localCursor;
    // Maps siteIds to a struct identifying a remote user
    // N.B. You can use it to iterate over all Accounts!    
    QMap<int, User> m_users;
    Ui::Editor *ui;

protected:
    //bool eventFilter(QObject *target, QEvent *event) override;

};

#endif // EDITOR_H
