QT += widgets
QT += printsupport

TEMPLATE        = app
TARGET          = polidox-client
QT += gui websockets core

INSTALLS += target

HEADERS += \
    CRDT.h \
    Changepwd.h \
    ClientController.h \
    CrdtClient.h \
    CrdtClient.h \
    Char.h \
    Creators.h \
    Editor.h \
    ClientMessageFactory.h \
    Account.h \
    ListFiles.h \
    Client.h \
    NewFileDialog.h \
    Profile.h \
    InsertUriDialog.h \
    ShowUriDialog.h \
    LoginWindow.h

SOURCES += \
    CRDT.cpp \
    Changepwd.cpp \
    ClientController.cpp \
    CrdtClient.cpp \
    Creators.cpp \
    Editor.cpp \
    ListFiles.cpp \
    Account.cpp \
    NewFileDialog.cpp \
    Profile.cpp \
    main.cpp \
    Char.cpp \
    ClientMessageFactory.cpp \
    Client.cpp \
    InsertUriDialog.cpp \
    ShowUriDialog.cpp \
    LoginWindow.cpp

DISTFILES += \
    images/exportpdf.png \
    images/filenew.png \
    images/new_file.png \
    images/open_file.png

FORMS += \
    changepwd.ui \
    creators.ui \
    editor.ui \
    listfiles.ui \
    newfiledialog.ui \
    inserturidialog.ui \
    profile.ui \
    showuridialog.ui \
    loginwindow.ui

RESOURCES += \
    Resources.qrc
