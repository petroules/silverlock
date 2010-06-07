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
    group.cpp
HEADERS += silverlocklib_global.h \
    database.h \
    entry.h \
    group.h \
    version.h
FORMS += 
OTHER_FILES += silverlocklib.rc
DESTDIR = ../bin

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlocklib.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
