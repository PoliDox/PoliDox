
#include <QApplication>
#include <QDesktopWidget>
#include "clientcontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
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

    return a.exec();
}
