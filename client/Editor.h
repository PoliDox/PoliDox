#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include <QMap>
#include <QLabel>
#include "Account.h"
#include "Styler.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Editor; }
QT_END_NAMESPACE

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
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    void init(const QString &p_text);
    QChar at(int pos);
    void addClient(const Account& user);
    void handleRemoteOperation(EditOp op, int siteId, int position, char ch = 0);
    //void remoteInsert(int siteId, int position, char ch);
    //void remoteDelete(int siteId, int position);
    void updateCursors(EditOp operation, int position, int siteId = -1); // siteId -1 identifies the local user

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);

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

private:
    void createActions();
    void createStatusBar();

    void setRichTextToolBar();

    bool handlingRemoteOp;
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_localCursor;
    // Maps siteIds to a struct identifying a remote user
    // N.B. You can use it to iterate over all Accounts!    
    QMap<int, User> m_users;
    Ui::Editor *ui;
};

#endif // EDITOR_H
