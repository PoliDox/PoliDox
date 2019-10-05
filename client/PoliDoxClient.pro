QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets

INSTALLS += target

HEADERS += \
    CRDTclient.h \
    editor.h \
    Char.h \
    clientcontroller.h

SOURCES += \
    CRDTclient.cpp \
    editor.cpp \
    main.cpp \
    Char.cpp \
    clientcontroller.cpp
