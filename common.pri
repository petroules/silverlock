include($$PWD/../petroules/qt/petroules-common.pri)

# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500

# -------------------------------------------------
# Include and build paths for external libraries
# -------------------------------------------------

LIEL_HEADERS = $$PWD/../liel/c++/liel
LIEL_BUILD = $$PWD/../liel/liel-build-desktop/lib
LIEL_LIB = liel
LIEL_VERSION = 1

BOTAN_HEADERS = $$PWD/../3rdparty/botan/build
BOTAN_BUILD = $$PWD/../3rdparty/botan-build-desktop/lib
BOTAN_LIB = Botan

QSA_HEADERS = $$PWD/../3rdparty/qtsingleapplication/src
QSA_BUILD = $$PWD/../3rdparty/qtsingleapplication/lib
QSA_LIB = qsa-2.6
