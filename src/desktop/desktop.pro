# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT += core gui network xml

# Desktop-only
win32|macx|unix {
    !android:!symbian:!maemo5 {
        QT += svg
        TEMPLATE = app
    }
}

TARGET = silverlock
symbian:TARGET.UID3 = 0xE9A31305

# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500

# Smart Installer package's UID
# This UID is from the protected range
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the
# MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

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

SOURCES += main.cpp \
    dialogs/welcomedialog.cpp

# Mobile-specific sources
android|symbian|maemo5|wince* {
    HEADERS += \
        mobile/mainwindow.h
    SOURCES += \
        mobile/mainwindow.cpp
    FORMS += \
        mobile/mainwindow.ui
} else {
    # Desktop platform sources
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
        widgets/toolbarsearchwidget.h \
        silverlockapplication.h \
        applicationmenu.h
    SOURCES += \
        databaseprinter.cpp \
        databaseprinterfields.cpp \
        documentstate.cpp \
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
}

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

win32:LIBS += -luser32
macx:LIBS += -framework CoreFoundation -framework Cocoa

SILVERLOCKLIB_PATH = ../libqt
PETROULESUTILITIES_PATH = ../petroules-utilities-qt/src
QTSOLUTIONS_PATH = $$PETROULESUTILITIES_PATH/../lib/qtsingleapplication/src

# Silverlock support library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/release/ -lsilverlocklib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/debug/ -lsilverlocklib
else:symbian: LIBS += -lsilverlocklib
else:unix: LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/ -lsilverlocklib

INCLUDEPATH += $$PWD/$$SILVERLOCKLIB_PATH
DEPENDPATH += $$PWD/$$SILVERLOCKLIB_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$SILVERLOCKLIB_PATH/libsilverlocklib.a

# Petroules Utilities library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/release/ -lpetroules-utilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/debug/ -lpetroules-utilities
else:symbian: LIBS += -lpetroules-utilities
else:unix: LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/ -lpetroules-utilities

# We have to make sure we include the QtSingleApplication headers
# path because it will get indirectly included from THIS project
INCLUDEPATH += $$PWD/$$PETROULESUTILITIES_PATH $$PWD/$$QTSOLUTIONS_PATH
DEPENDPATH += $$PWD/$$PETROULESUTILITIES_PATH $$PWD/$$QTSOLUTIONS_PATH

macx:PRE_TARGETDEPS += $$OUT_PWD/$$PETROULESUTILITIES_PATH/libpetroules-utilities.a

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

win32:RC_FILE = silverlock.rc
macx:ICON = ../../res/app.icns
macx:QMAKE_INFO_PLIST = Info.plist

# Show the console when debugging on Windows
win32:CONFIG(debug, debug|release):CONFIG += console

# Add files and directories to ship with the application
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    dialogs/welcomedialog.h

FORMS += \
    dialogs/welcomedialog.ui



