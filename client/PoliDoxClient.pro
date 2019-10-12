QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets

INSTALLS += target

HEADERS += \
    ClientController.h \
    CrdtClient.h \
    CrdtClient.h \
    Char.h \
    Editor.h \
    MessageFactory.h

SOURCES += \
    ClientController.cpp \
    CrdtClient.cpp \
    Editor.cpp \
    main.cpp \
    Char.cpp \
    MessageFactory.cpp

DISTFILES += \
    images/filenew.png \
    images/new_file.png \
    images/open_file.png

FORMS += \
    editor.ui

RESOURCES += \
    Resources.qrc
