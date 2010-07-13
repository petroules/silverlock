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
    groupnode.cpp \
    databasenode.cpp \
    itemnode.cpp \
    databasereader.cpp \
    databasewriter.cpp
HEADERS += silverlocklib_global.h \
    database.h \
    entry.h \
    group.h \
    version.h \
    silverlocklib.h \
    qversion.h \
    database_keys.h \
    groupnode.h \
    databasenode.h \
    itemnode.h \
    databasereader.h \
    databasewriter.h
FORMS += 
OTHER_FILES += silverlocklib.rc
DESTDIR = ../bin

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlocklib.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
