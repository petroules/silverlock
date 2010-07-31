# -------------------------------------------------
# Project created by QtCreator 2010-04-08T06:08:50
# -------------------------------------------------
QT += network \
    svg \
    xml
TARGET = silverlock
TEMPLATE = app
# We want Windows methods from at least Windows 2000 (NT 5.0)
DEFINES += WINVER=0x0500
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    entryeditdialog.cpp \
    groupeditdialog.cpp \
    preferencesdialog.cpp \
    silverlockpreferences.cpp \
    groupbrowserwidget.cpp \
    entrytablewidget.cpp \
    selectgroupdialog.cpp \
    searchdatabasedialog.cpp \
    documentstate.cpp \
    configurecolumnsdialog.cpp \
    databaseauthenticationdialog.cpp \
    aboutdialog.cpp \
    licensedialog.cpp \
    updatedialog.cpp \
    guardeddialog.cpp \
    inactivityeventfilter.cpp \
    entryeditdialog_helper.cpp \
    databaseprintdialog.cpp
HEADERS += \
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
    searchdatabasedialog.h \
    documentstate.h \
    configurecolumnsdialog.h \
    entryviewindexes.h \
    databaseauthenticationdialog.h \
    aboutdialog.h \
    licensedialog.h \
    updatedialog.h \
    guardeddialog.h \
    stable.h \
    inactivityeventfilter.h \
    databaseprintdialog.h
PRECOMPILED_HEADER = stable.h
FORMS += mainwindow.ui \
    entryeditdialog.ui \
    groupeditdialog.ui \
    preferencesdialog.ui \
    groupbrowserwidget.ui \
    entrytablewidget.ui \
    selectgroupdialog.ui \
    searchdatabasedialog.ui \
    configurecolumnsdialog.ui \
    databaseauthenticationdialog.ui \
    aboutdialog.ui \
    licensedialog.ui \
    updatedialog.ui \
    databaseprintdialog.ui
RESOURCES += resources.qrc \
    ../res/globalresources.qrc
DESTDIR = ../bin
INCLUDEPATH += ../silverlocklib ../../liel/c++/liel ../qtsingleapplication/src
LIBS += -L../bin -L../../liel/liel-build-desktop/bin -lQtSolutions_SingleApplication-2.6
win32:LIBS += -lsilverlocklib1 -lliel1
macx:LIBS += -lsilverlocklib.1 -lliel.1
linux-g++:LIBS += -lsilverlocklib -lliel
OTHER_FILES += botan.txt silverlock.rc \
    silverlock.manifest

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlock.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
