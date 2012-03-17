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
    databaseprinter.h \
    databaseprinterfields.h \
    documentstate.h \
    mainwindow.h \
    silverlockpreferences.h \
    silverlockpreferences_keys.h \
    stable.h \
    version.h \
    dialogs/aboutdialog.h \
    dialogs/configurecolumnsdialog.h \
    dialogs/databaseauthenticationdialog.h \
    dialogs/databaseprintpreviewdialog.h \
    dialogs/entryeditdialog.h \
    dialogs/groupeditdialog.h \
    dialogs/guardeddialog.h \
    dialogs/licensedialog.h \
    dialogs/newdatabasewizard.h \
    dialogs/preferencesdialog.h \
    dialogs/searchdatabasedialog.h \
    dialogs/selectgroupdialog.h \
    dialogs/updatedialog.h \
    widgets/entrytablewidget.h \
    widgets/entryviewindexes.h \
    widgets/expandingspacerwidget.h \
    widgets/groupbrowserwidget.h \
    widgets/itoolbarsearchwidget.h \
    widgets/toolbarsearchwidget.h \
    silverlockapplication.h \
    applicationmenu.h
SOURCES += \
    databaseprinter.cpp \
    databaseprinterfields.cpp \
    documentstate.cpp \
    main.cpp \
    mainwindow.cpp \
    silverlockpreferences.cpp \
    dialogs/aboutdialog.cpp \
    dialogs/configurecolumnsdialog.cpp \
    dialogs/databaseauthenticationdialog.cpp \
    dialogs/databaseprintpreviewdialog.cpp \
    dialogs/entryeditdialog.cpp \
    dialogs/entryeditdialog_helper.cpp \
    dialogs/groupeditdialog.cpp \
    dialogs/guardeddialog.cpp \
    dialogs/licensedialog.cpp \
    dialogs/newdatabasewizard.cpp \
    dialogs/preferencesdialog.cpp \
    dialogs/searchdatabasedialog.cpp \
    dialogs/selectgroupdialog.cpp \
    dialogs/updatedialog.cpp \
    widgets/entrytablewidget.cpp \
    widgets/expandingspacerwidget.cpp \
    widgets/groupbrowserwidget.cpp \
    widgets/itoolbarsearchwidget.cpp \
    widgets/toolbarsearchwidget.cpp \
    silverlockapplication.cpp \
    applicationmenu.cpp \
    mainwindow_menuhandlers.cpp
FORMS += \
    mainwindow.ui \
    dialogs/aboutdialog.ui \
    dialogs/configurecolumnsdialog.ui \
    dialogs/databaseauthenticationdialog.ui \
    dialogs/databaseprintpreviewdialog.ui \
    dialogs/entryeditdialog.ui \
    dialogs/groupeditdialog.ui \
    dialogs/licensedialog.ui \
    dialogs/newdatabasewizard.ui \
    dialogs/preferencesdialog.ui \
    dialogs/searchdatabasedialog.ui \
    dialogs/selectgroupdialog.ui \
    dialogs/updatedialog.ui \
    widgets/entrytablewidget.ui \
    widgets/groupbrowserwidget.ui \
    widgets/toolbarsearchwidget.ui
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

# Mac-specific sources
macx {
    HEADERS += \
        mac/icocoaeventreceiver.h \
        mac/macloginitemsmanager.h \
        mac/mactoolbarsearchwidget.h
    OBJECTIVE_HEADERS += \
        mac/cocoaeventlistener.h \
        mac/machelpers.h
    OBJECTIVE_SOURCES += \
        mac/cocoaeventlistener.mm \
        mac/icocoaeventreceiver.mm \
        mac/machelpers.mm \
        mac/macloginitemsmanager.mm \
        mac/mactoolbarsearchwidget.mm
}

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

win32:LIBS += -luser32
macx:LIBS += -framework CoreFoundation -framework Cocoa

win32:TEMP_BUILDDIR = temp-win32-gcc
win32-msvc*:TEMP_BUILDDIR = temp-win32-msvc
macx:TEMP_BUILDDIR = temp-mac64
linux-g++:TEMP_BUILDDIR = temp-linux32

SILVERLOCKLIB_PATH = ../libqt
SYNTEZA_PATH = ../3rdparty/$$TEMP_BUILDDIR/synteza/qt
BOTAN_PATH = ../3rdparty/$$TEMP_BUILDDIR/botan

# Silverlock support library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/release/ -lsilverlocklib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/debug/ -lsilverlocklib
else:symbian: LIBS += -lsilverlocklib
else:unix: LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/ -lsilverlocklib

INCLUDEPATH += $$PWD/$$SILVERLOCKLIB_PATH
DEPENDPATH += $$PWD/$$SILVERLOCKLIB_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$SILVERLOCKLIB_PATH/libsilverlocklib.a

# Synteza library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SYNTEZA_PATH/release/ -lsynteza
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SYNTEZA_PATH/debug/ -lsynteza
else:symbian: LIBS += -lsynteza
else:unix: LIBS += -L$$OUT_PWD/$$SYNTEZA_PATH/ -lsynteza

INCLUDEPATH += $$PWD/$$SYNTEZA_PATH
DEPENDPATH += $$PWD/$$SYNTEZA_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$SYNTEZA_PATH/libsynteza.a

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
