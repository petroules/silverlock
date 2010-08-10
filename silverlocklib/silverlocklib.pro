# -------------------------------------------------
# Project created by QtCreator 2010-04-08T15:35:27
# -------------------------------------------------
include(../common.pri)
include(version.pri)

QT += network \
    xml
TARGET = $$APP_UNIXNAME
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
    silverlocklib.h \
    database_keys.h \
    databasenode.h \
    databasereader.h \
    databasewriter.h \
    searchparameters.h \
    databasecrypto.h \
    stable.h \
    version.h
PRECOMPILED_HEADER = stable.h
TRANSLATIONS += tr/silverlocklib_de.ts \
    tr/silverlocklib_fr.ts
OTHER_FILES += silverlocklib.rc \
    version.pri
DESTDIR = ../lib
INCLUDEPATH += $$LIEL_HEADERS $$BOTAN_HEADERS
LIBS += -L$$LIEL_BUILD -l$$platformversion($$LIEL_LIB, $$LIEL_VERSION) -L$$BOTAN_BUILD -l$$BOTAN_LIB

win32 {
    RC_FILE = silverlocklib.rc
}

macx {
    ICON = res/app.icns

    # Necessary for macdeployqt to work (DESTDIR must also end in /lib)
    CONFIG += absolute_library_soname
    target.path = $$OUT_PWD/$$DESTDIR
    INSTALLS += target
}
