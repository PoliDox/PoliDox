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
    ListFiles.h \
    Log_dialog.h \
    Client.h \
    NewFileDialog.h \
    Styler.h

SOURCES += \
    CRDT.cpp \
    ClientController.cpp \
    CrdtClient.cpp \
    Editor.cpp \
    ListFiles.cpp \
    Log_dialog.cpp \
    Account.cpp \
    NewFileDialog.cpp \
    Styler.cpp \
    main.cpp \
    Char.cpp \
    ClientMessageFactory.cpp \
    Client.cpp

DISTFILES += \
    images/filenew.png \
    images/new_file.png \
    images/open_file.png

FORMS += \
    editor.ui \
    listfiles.ui \
    log_dialog.ui \
    newfiledialog.ui

RESOURCES += \
    Resources.qrc
