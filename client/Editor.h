#ifndef EDITOR_H
#define EDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include <QMap>
#include <QLabel>
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
    void addClient(int siteId);

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

    bool handlingRemoteOp;
    QTextEdit *m_textEdit;
    QTextDocument *m_textDoc;
    QTextCursor *m_localCursor;
    QMap<int, QLabel*> m_remoteCursors;
    Ui::Editor *ui;
};

#endif // EDITOR_H
