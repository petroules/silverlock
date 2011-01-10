# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT += core gui network svg webkit xml
TEMPLATE = app
TARGET = silverlock

# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500

# --------------------------------------------------
# This section contains all the main code/resource
# files that make up the target, including:
# * precompiled header (.pch)
# * headers (.h)
# * sources (.c, .cpp, .m, .mm, ...)
# * forms (.ui)
# * resources (.qrc)
# * translations (.tr)
# * any other files belonging in the OTHER_FILES var
# --------------------------------------------------

PRECOMPILED_HEADER = stable.h
HEADERS += \
    mainwindow.h \
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
    databaseprintdialog.h \
    version.h \
    newdatabasewizard.h
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
    databaseprintdialog.cpp \
    newdatabasewizard.cpp
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
    databaseprintdialog.ui \
    newdatabasewizard.ui
RESOURCES += \
    resources.qrc \
    ../../res/globalresources.qrc
TRANSLATIONS += \
    tr/silverlock_de.ts \
    tr/silverlock_fr.ts
OTHER_FILES += \
    silverlock.rc \
    silverlock.manifest \
    Info.plist

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

win32:LIBS += -luser32
macx:LIBS += -framework CoreFoundation -framework Cocoa

!include(../3rdparty/temp/qtsolutions/qtsingleapplication/src/qtsingleapplication.pri) {
    error("Could not find the qtsingleapplication.pri file! Have you run configure in the 3rdparty directory?")
}

SILVERLOCKLIB_PATH = ../libqt
LIEL_PATH = ../3rdparty/temp/liel/qt/liel
BOTAN_PATH = ../3rdparty/temp/botan
win32-msvc*:BOTAN_PATH = $$BOTAN_PATH-msvc

# Silverlock support library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/release/ -lsilverlocklib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/debug/ -lsilverlocklib
else:symbian: LIBS += -lsilverlocklib
else:unix: LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/ -lsilverlocklib

INCLUDEPATH += $$PWD/$$SILVERLOCKLIB_PATH
DEPENDPATH += $$PWD/$$SILVERLOCKLIB_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$SILVERLOCKLIB_PATH/libsilverlocklib.a

# LIEL library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$LIEL_PATH/release/ -lliel
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$LIEL_PATH/debug/ -lliel
else:symbian: LIBS += -lliel
else:unix: LIBS += -L$$OUT_PWD/$$LIEL_PATH/ -lliel

INCLUDEPATH += $$PWD/$$LIEL_PATH
DEPENDPATH += $$PWD/$$LIEL_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$LIEL_PATH/libliel.a

# Botan library

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/$$BOTAN_PATH/ -lbotan
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/$$BOTAN_PATH/ -lbotand
else:symbian: LIBS += -lbotan
else:unix: LIBS += -L$$PWD/$$BOTAN_PATH/ -lbotan

INCLUDEPATH += $$PWD/$$BOTAN_PATH/build/include
DEPENDPATH += $$PWD/$$BOTAN_PATH/build/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/$$BOTAN_PATH/botan.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/$$BOTAN_PATH/botand.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/$$BOTAN_PATH/libbotan.a

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

win32:RC_FILE = silverlock.rc
macx:ICON = ../../res/app.icns
macx:QMAKE_INFO_PLIST = Info.plist

# Show the console when debugging on Windows
win32:CONFIG(debug, debug|release):CONFIG += console
