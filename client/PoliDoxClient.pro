QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets

INSTALLS += target

HEADERS += \
    CLIENTcontroller.h \
    CRDTclient.h \
    editor.h \
    Char.h

SOURCES += \
    CLIENTcontroller.cpp \
    CRDTclient.cpp \
    editor.cpp \
    main.cpp \
    Char.cpp
