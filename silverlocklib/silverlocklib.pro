# -------------------------------------------------
# Project created by QtCreator 2010-04-08T15:35:27
# -------------------------------------------------
QT += network \
    xml
TARGET = silverlocklib
TEMPLATE = lib
DEFINES += SILVERLOCKLIB_LIBRARY
SOURCES += database.cpp \
    entry.cpp \
    group.cpp \
    qversion.cpp \
    databasenode.cpp \
    databasereader.cpp \
    databasewriter.cpp \
    searchparameters.cpp \
    databasecrypto.cpp
HEADERS += silverlocklib_global.h \
    database.h \
    entry.h \
    group.h \
    version.h \
    silverlocklib.h \
    qversion.h \
    database_keys.h \
    databasenode.h \
    databasereader.h \
    databasewriter.h \
    searchparameters.h \
    databasecrypto.h
FORMS +=
OTHER_FILES += silverlocklib.rc
win32:INCLUDEPATH += ../botan/build
win32:debug:LIBS += -L../botan/src/debug -lbotan
win32:release:LIBS += -L../botan/src/release -lbotan
DESTDIR = ../bin

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlocklib.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
