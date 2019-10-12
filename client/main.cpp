
#include <QApplication>
#include <QDesktopWidget>
#include "ClientController.h"
#include "Editor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PoliDox");
    QCoreApplication::setApplicationName("PoliDox client");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    ClientController l_controller;

    return app.exec();
}
