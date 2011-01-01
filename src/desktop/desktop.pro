include(../common.pri)
include(version.pri)

# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT += core gui network svg xml
TEMPLATE = app
TARGET = $$APP_UNIXNAME

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
    version.h
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
RESOURCES += \
    resources.qrc \
    ../res/globalresources.qrc
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

!include(../3rdparty/temp/qtsolutions/qtsingleapplication/src/qtsingleapplication.pri) {
    error("Could not find the qtsingleapplication.pri file! Have you run configure in the 3rdparty directory?")
}

SILVERLOCKLIB_PATH = ../silverlocklib
LIEL_PATH = ../liel

# Silverlock support library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/release/ -lsilverlocklib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/debug/ -lsilverlocklib
else:symbian: LIBS += -lsilverlocklib
else:unix: LIBS += -L$$OUT_PWD/$$SILVERLOCKLIB_PATH/ -lsilverlocklib

INCLUDEPATH += $$PWD/$$SILVERLOCKLIB_PATH
DEPENDPATH += $$PWD/$$SILVERLOCKLIB_PATH

# LIEL library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$LIEL_PATH/release/ -lliel
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$LIEL_PATH/debug/ -lliel
else:symbian: LIBS += -lliel
else:unix: LIBS += -L$$OUT_PWD/$$LIEL_PATH/ -lliel

INCLUDEPATH += $$PWD/$$LIEL_PATH
DEPENDPATH += $$PWD/$$LIEL_PATH

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

win32:RC_FILE = silverlock.rc
macx:ICON = res/app.icns

# Show the console when debugging on Windows
win32:CONFIG(debug, debug|release):CONFIG += console

# --------------------------------------------------
# This section contains commands for deployment to
# various platforms, especially mobile devices
# --------------------------------------------------

# Copy over dependent libraries - not necessary on Mac OS X as macdeployqt will take care of it...
#!macx {
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$OUT_PWD/../lib/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$LIEL_BUILD/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$BOTAN_BUILD/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
#}

#macx {
#    # Use our custom plist file and populate it with the correct information
#    QMAKE_INFO_PLIST = Info.plist
#    QMAKE_POST_LINK += $$populateplist()

#    # Run macdeployqt and rename the bundle to its friendly name
#    QMAKE_POST_LINK += $$macdeployqt(3) $$renamebundle()
#}

#linux* {
#    # Copy launcher shell script and make executable
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$PWD/../deploy/linux/launcher.sh) $$formatpath($$OUT_PWD/$$DESTDIR/$${TARGET}.sh) $$CMD_SEP
#    QMAKE_POST_LINK += chmod +x $$formatpath($${OUT_PWD}/$${DESTDIR}/$${TARGET}.sh) $$CMD_SEP

#    # Copy desktop entry file
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$PWD/../deploy/linux/launcher.desktop) $$formatpath($$OUT_PWD/$$DESTDIR/$${TARGET}.desktop) $$CMD_SEP

#    # Copy application icon
#    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$PWD/res/app.svg) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
#}
