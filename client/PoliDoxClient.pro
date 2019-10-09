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
    Editor.h

SOURCES += \
    ClientController.cpp \
    CrdtClient.cpp \
    Editor.cpp \
    main.cpp \
    Char.cpp
