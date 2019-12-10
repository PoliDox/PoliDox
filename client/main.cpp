
#include <QApplication>
#include <QDesktopWidget>
#include "Client.h"
#include "Editor.h"
#include "QString"

int main(int argc, char *argv[])
{
    try {

        QString ip_address("127.0.0.1");
        if (argc == 2){
            ip_address = QString(argv[1]);
        }
        QApplication app(argc, argv);
        QCoreApplication::setOrganizationName("PoliDox");
        QCoreApplication::setApplicationName("PoliDox");
        QCoreApplication::setApplicationVersion(QT_VERSION_STR);

        Client l_client(ip_address);

        app.exec();

    } catch (...) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
