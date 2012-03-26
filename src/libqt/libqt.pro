# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT += core gui network xml
macx:CONFIG += staticlib
TEMPLATE = lib
DEFINES += SILVERLOCKLIB_LIBRARY
TARGET = silverlocklib

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
    silverlocklib_global.h \
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
    version.h \
    botan/botan_all.h
SOURCES += \
    database.cpp \
    entry.cpp \
    group.cpp \
    databasenode.cpp \
    databasereader.cpp \
    databasewriter.cpp \
    searchparameters.cpp \
    databasecrypto.cpp \
    botan/botan_all.cpp
RESOURCES +=
TRANSLATIONS += \
    tr/silverlocklib_de.ts \
    tr/silverlocklib_fr.ts
OTHER_FILES += \
    silverlocklib.rc

android:SOURCES += jni.cpp

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

# Botan needs some stuff from here
win32:LIBS += -ladvapi32 -luser32

PETROULESUTILITIES_PATH = ../petroules-utilities-qt/src
QTSOLUTIONS_PATH = $$PETROULESUTILITIES_PATH/../lib/qtsingleapplication/src

# Petroules Utilities library

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/release/ -lpetroules-utilities
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/debug/ -lpetroules-utilities
else:symbian:LIBS += -lpetroules-utilities
else:unix:LIBS += -L$$OUT_PWD/$$PETROULESUTILITIES_PATH/ -lpetroules-utilities

# We have to make sure we include the QtSingleApplication headers
# path because it will get indirectly included from THIS project
INCLUDEPATH += $$PWD/$$PETROULESUTILITIES_PATH $$PWD/$$QTSOLUTIONS_PATH
DEPENDPATH += $$PWD/$$PETROULESUTILITIES_PATH $$PWD/$$QTSOLUTIONS_PATH

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$$PETROULESUTILITIES_PATH/release/petroules-utilities.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$$PETROULESUTILITIES_PATH/debug/petroules-utilities.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/$$PETROULESUTILITIES_PATH/libpetroules-utilities.a

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

win32:RC_FILE = silverlocklib.rc
macx:ICON = ../../res/app.icns
android:QMAKE_CXXFLAGS += -fexceptions
#android:QMAKE_CXXFLAGS += -O3 -finline-functions -D_REENTRANT -Wno-long-long -fpermissive -W -Wall -marm -mno-thumb -fexceptions
#android:QMAKE_CXXFLAGS -= -mthumb # This gets inserted in the makefile by the toolchain and causes problems, but this doesn't remove it
message("C++ compiler flags are $$QMAKE_CXXFLAGS")

# --------------------------------------------------
# This section contains commands for deployment to
# various platforms, especially mobile devices
# --------------------------------------------------

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE1888399
    TARGET.CAPABILITY = NetworkServices
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = silverlocklib.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
