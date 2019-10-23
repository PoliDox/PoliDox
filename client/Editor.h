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

class Editor : public QMainWindow
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

    QChar at(int pos);
    void addClient(const Account& user);
    void remoteInsert(int siteId, int position, char ch);
    void remoteDelete(int siteId, int position);

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

private:
    void createActions();
    void createStatusBar();

    bool handlingRemoteOp;
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_localCursor;
    QVector<Account> m_users;
    QMap<int, QLabel*> m_remoteCursors;
    Ui::Editor *ui;
};

#endif // EDITOR_H
