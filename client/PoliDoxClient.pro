QT += widgets

TEMPLATE        = app
TARGET          = polidox-client
QT += gui

INSTALLS += target

HEADERS += \
    CRDTclient.h \
    editor.h \
    Char.h

SOURCES += \
    CRDTclient.cpp \
    editor.cpp \
    main.cpp \
    Char.cpp
