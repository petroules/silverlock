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
    groupeditdialog.cpp \
    preferencesdialog.cpp \
    silverlockpreferences.cpp \
    groupbrowserwidget.cpp \
    entrytablewidget.cpp \
    selectgroupdialog.cpp \
    searchdatabasedialog.cpp
HEADERS += aboutdialog.h \
    mainwindow.h \
    version.h \
    entryeditdialog.h \
    groupeditdialog.h \
    preferencesdialog.h \
    silverlockpreferences.h \
    groupbrowserwidget.h \
    entrytablewidget.h \
    silverlockpreferences_keys.h \
    selectgroupdialog.h \
    searchdatabasedialog.h
FORMS += mainwindow.ui \
    entryeditdialog.ui \
    groupeditdialog.ui \
    preferencesdialog.ui \
    groupbrowserwidget.ui \
    entrytablewidget.ui \
    selectgroupdialog.ui \
    searchdatabasedialog.ui
RESOURCES += resources.qrc
DESTDIR = ../bin
INCLUDEPATH += ../silverlocklib
win32:LIBS += -L../bin -lsilverlocklib1
macx:LIBS += -L../bin -lsilverlocklib.1
linux-g++:LIBS += -L../bin -lsilverlocklib
OTHER_FILES += botan.txt \
    silverlock.rc

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlock.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
