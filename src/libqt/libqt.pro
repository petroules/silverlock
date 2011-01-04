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
    version.h
SOURCES += \
    database.cpp \
    entry.cpp \
    group.cpp \
    databasenode.cpp \
    databasereader.cpp \
    databasewriter.cpp \
    searchparameters.cpp \
    databasecrypto.cpp
RESOURCES +=
TRANSLATIONS += \
    tr/silverlocklib_de.ts \
    tr/silverlocklib_fr.ts
OTHER_FILES += \
    silverlocklib.rc

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

# Botan needs some stuff from here
win32:LIBS += -ladvapi32 -luser32

!macx {
	LIEL_PATH = ../liel
	BOTAN_PATH = ../3rdparty/temp/botan
	win32-msvc*:BOTAN_PATH = $$BOTAN_PATH-msvc

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
}

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

win32:RC_FILE = silverlocklib.rc
macx:ICON = ../../res/app.icns

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
