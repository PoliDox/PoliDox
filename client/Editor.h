#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include <QMap>
#include <QLabel>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QFontComboBox>
#include "Account.h"
#include "Styler.h"
#include "ClientController.h"
#include "ShowUriDialog.h"


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
    explicit Editor(ClientController *p_controller, QWidget *parent = nullptr, const QList<Account>& contributorsOnline = {}, const QList<Account>& contributorsOffline = {});
    ~Editor() override;

    void init(const QString &p_text);
    QChar at(int pos);
    void addClient(const Account& user);
    void removeClient(const Account& account);
    void handleRemoteOperation(EditOp op, Char symbol, int position, int siteId);
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
    void on_actionURI_triggered();

    void on_actionBold_triggered();
    void on_actionItalic_triggered();
    void on_actionUnderlined_triggered();

    void onFontSizeChanged(int i);
    void onFontFamilyChanged(const QFont& font);
    void onCharFormatChanged(const QTextCharFormat & f);

    void on_actionLeftAllignmet_triggered();
    void on_actionAlignCenter_triggered();
    void on_actionAlignRight_triggered();
    void on_actionJustify_triggered(); 

    void highlightUser(QListWidgetItem * item);

private:
    void createActions();
    void createStatusBar();
    void initUserList();
    void initRichTextToolBar();
    Q_INVOKABLE void updateCursors();
    void highlightUserChars(int p_siteId, QColor p_color, bool p_checked);
    void bootContributorsLists(QList<Account> contributorsOnline, QList<Account> contributorsoffline);
    void addOnlineUser(const Account& account);
    void addOfflineUser(const Account& account);

    ClientController *controller;
    bool handlingOperation;
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_localCursor;
    // Maps siteIds to a struct identifying a remote user
    // N.B. You can use it to iterate over all Accounts!    
    QMap<int, User> m_onlineUsers;
    QList<Account> m_offlineUsers;
    ShowUriDialog *m_showUriDialog;
    Ui::Editor *ui;
    // TODO Orribile: spostare in editor.ui
    QFontComboBox* m_font;
    QSpinBox* m_fontSize;

protected:
    //bool eventFilter(QObject *target, QEvent *event) override;

};

#endif // EDITOR_H
