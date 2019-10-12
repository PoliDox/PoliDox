#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include "CrdtClient.h"


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

signals:
    void textChanged(int position, int charsRemoved, int charsAdded);

public slots:
    void remoteInsert(int position, char ch);
    void remoteDelete(int position);
    //void newFile();


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

private:
    void createActions();
    void createStatusBar();
    void readSettings();
    void setCurrentFile(const QString &fileName);
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_remoteCursor;
    bool handlingRemoteOp;
    QString curFile;
    Ui::Editor *ui;
};

#endif // EDITOR_H
