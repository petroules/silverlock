#-------------------------------------------------
#
# Project created by QtCreator 2011-06-12T21:32:16
#
#-------------------------------------------------

QT       += testlib xml

TARGET = tst_libqttesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_libqttesttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"









win32:TEMP_BUILDDIR = temp-win32-gcc
win32-msvc*:TEMP_BUILDDIR = temp-win32-msvc
macx:TEMP_BUILDDIR = temp-mac64
linux-g++:TEMP_BUILDDIR = temp-linux32

SILVERLOCKLIB_PATH = ../libqt
SYNTEZA_PATH = ../3rdparty/$$TEMP_BUILDDIR/synteza/qt
BOTAN_PATH = ../3rdparty/$$TEMP_BUILDDIR/botan
win32-msvc*:BOTAN_PATH = $$BOTAN_PATH-msvc

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
