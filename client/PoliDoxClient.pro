QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets

INSTALLS += target

HEADERS += \
    ClientController.h \
    CrdtClient.h \
    Editor.h \
    Char.h

SOURCES += \
    ClientController.cpp \
    Crdtclient.cpp \
    Editor.cpp \
    main.cpp \
    Char.cpp
