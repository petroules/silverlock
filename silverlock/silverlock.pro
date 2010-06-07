# -------------------------------------------------
# Project created by QtCreator 2010-04-08T06:08:50
# -------------------------------------------------
QT += network \
    webkit \
    xml
TARGET = silverlock
TEMPLATE = app
SOURCES += aboutdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    entryeditdialog.cpp \
    groupeditdialog.cpp
HEADERS += aboutdialog.h \
    mainwindow.h \
    version.h \
    entryeditdialog.h \
    groupeditdialog.h
FORMS += mainwindow.ui \
    entryeditdialog.ui \
    groupeditdialog.ui
RESOURCES += resources.qrc
DESTDIR = ../bin
INCLUDEPATH += ../silverlocklib
!mac:LIBS += -L../bin \
    -lsilverlocklib1
mac:LIBS += -L../bin \
    -lsilverlocklib.1
OTHER_FILES += botan.txt \
    silverlock.rc

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlock.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
