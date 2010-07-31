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
    database_keys.h \
    databasenode.h \
    databasereader.h \
    databasewriter.h \
    searchparameters.h \
    databasecrypto.h \
    stable.h
PRECOMPILED_HEADER = stable.h
FORMS +=
OTHER_FILES += silverlocklib.rc
DESTDIR = ../bin
INCLUDEPATH += ../../liel/c++/liel ../botan/build
LIBS += -L../../liel/liel-build-desktop/bin -L../bin -lBotan
win32:LIBS += -lliel1
macx:LIBS += -lliel.1
linux-g++:LIBS += -lliel

# General information
VERSION = 1.0

# Resource file for Windows
RC_FILE = silverlocklib.rc

# Icon for Mac OS X application bundle
ICON = res/app.icns
