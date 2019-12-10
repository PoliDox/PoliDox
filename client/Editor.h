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
#include "ClientController.h"
#include "ShowUriDialog.h"
#include "Profile.h"


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

#define DEFAULT_FONT "Lucida Grande"
#define DEFAULT_SIZE 12

class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor(ClientController *p_controller, QWidget *parent = nullptr, const QList<Account>& contributorsOnline = {}, const QList<Account>& contributorsOffline = {}, const Account* main_account = nullptr);
    ~Editor() override;

    void init(const QVector<Char>& p_text);
    QChar at(int pos);
    int textSize();
    void addClient(const Account& user);
    void removeClient(const Account& account);
    void handleRemoteOperation(EditOp op, Char& symbol, int position, int siteId);
    void resetBackgroundColor(int pos, const QColor& color);
    void setCharacterStyle(int index, Char& symbol);
    void resetActionToggle();
    void moveCursor(int pos, int siteId);
    void closeEvent(QCloseEvent *event) override;
    void setNewImage(const QPixmap& Pix);
    QColor userSelected(int siteId);
    Profile *getProfilePtr();

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);
    void cursorPositionChanged(int position);
    void quit_editor();
    void quit_editor_new_file();
    void ChangeImgEditor(QPixmap& pix);
    void ChangePwdEditor(QString& pwd);

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_as_triggered();
    void on_actionCopy_triggered();
    void on_actionQuit_triggered();
    void on_actionPaste_triggered();
    void on_actionCut_triggered();
    void on_actionURI_triggered();
    void on_actionBold_triggered();
    void on_actionItalic_triggered();
    void on_actionUnderlined_triggered();
    void on_actionAlignLeft_triggered();
    void on_actionAlignCenter_triggered();
    void on_actionAlignRight_triggered();
    void on_actionJustify_triggered(); 
    void actionAccount_triggered(bool checked=false);

    void onFontSizeChanged(int i);
    void onFontFamilyChanged(const QFont& font);
    void onCharFormatChanged(const QTextCharFormat & f);
    void highlightUser(QListWidgetItem * item);


    void on_actioncreators_triggered();

private:
    void createActions();
    void createStatusBar();
    void initContributorsLists();
    void initRichTextToolBar();
    Q_INVOKABLE void updateCursors();
    void highlightUserChars(int p_siteId, QColor p_color, bool p_checked);
    void bootContributorsLists(QList<Account> contributorsOnline, QList<Account> contributorsoffline);
    void addOnlineUser(const Account& account);
    void addOfflineUser(const Account& account);
    void addChar(const Char& p_char);
    void addChar(const Char& p_char,QTextCursor& p_cursor);
    void assignRandomColor(int siteID);
    void setItem(QColor color, QListWidgetItem* item); // Technically not a member function

    ClientController *controller = nullptr;
    QTextEdit *m_textEdit = nullptr;
    QTextDocument *m_textDoc = nullptr;
    QTextCursor *m_localCursor = nullptr;

    QMap<int, User> m_onlineUsers; // maps siteIds to a struct identifying a remote user
    QMap<int,QString> assignedColor;
    QList<Account> m_offlineUsers;

    ShowUriDialog *m_showUriDialog = nullptr;
    Profile *profile = nullptr;
    Ui::Editor *ui = nullptr;

    bool handlingOperation;
    bool localOperation;
    bool changingFormat;
    bool alignFlag;
};

#endif // EDITOR_H
