
#include <QApplication>
#include <QDesktopWidget>
#include "Client.h"
#include "Editor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PoliDox");
    QCoreApplication::setApplicationName("PoliDox");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    Client l_client;

    return app.exec();
}
