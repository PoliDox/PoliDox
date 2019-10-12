
#include <QApplication>
#include <QDesktopWidget>
#include "ClientController.h"

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PoliDox");
    QCoreApplication::setApplicationName("PoliDox client");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    ClientController l_controller;

    /*
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(&editor);
    editor.resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    editor.move((availableGeometry.width() - editor.width()) / 2,
            (availableGeometry.height() - editor.height()) / 2);

    if (!editor.load(parser.positionalArguments().value(0, QLatin1String(":/example.html"))))
        editor.fileNew();
    */

   /* MainWindow mainWin;
    mainWin.show();*/

    return app.exec();
}
