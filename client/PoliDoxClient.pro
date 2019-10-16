QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets

INSTALLS += target

HEADERS += \
    CRDT.h \
    ClientController.h \
    CrdtClient.h \
    CrdtClient.h \
    Char.h \
    Editor.h \
    ClientMessageFactory.h \
    Account.h \
    Log_dialog.h

SOURCES += \
    CRDT.cpp \
    ClientController.cpp \
    CrdtClient.cpp \
    Editor.cpp \
    Log_dialog.cpp \
    main.cpp \
    Char.cpp \
    ClientMessageFactory.cpp

DISTFILES += \
    images/filenew.png \
    images/new_file.png \
    images/open_file.png

FORMS += \
    editor.ui \
    log_dialog.ui \

RESOURCES += \
    Resources.qrc
