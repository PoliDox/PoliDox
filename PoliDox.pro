QT += widgets

TEMPLATE        = app
TARGET          = polidox
QT += gui

INSTALLS += target

HEADERS += \
    CRDTclient.h \
    Char.h \
    editor.h \
    symbol.h

SOURCES += \
    CRDTclient.cpp \
    Char.cpp \
    editor.cpp \
    main.cpp \
    symbol.cpp
