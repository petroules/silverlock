# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT += testlib xml
TEMPLATE = app
TARGET = tst_libqttesttest
CONFIG   += console
CONFIG   -= app_bundle

DEFINES += SRCDIR=\\\"$$PWD/\\\"

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

SOURCES += tst_libqttesttest.cpp

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

SILVERLOCKLIB_PATH = ../libqt
PETROULESUTILITIES_PATH = ../petroules-utilities-qt/src
QTSOLUTIONS_PATH = $$PETROULESUTILITIES_PATH/../lib/qtsolutions/qtsingleapplication/src

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
