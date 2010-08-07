include(version.pri)

HEADERS += ../version.h
OTHER_FILES += ../version.pri

# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500

# -------------------------------------------------
# Include and build paths for external libraries
# -------------------------------------------------

LIEL_HEADERS = $$PWD$$/../liel/c++/liel
LIEL_BUILD = $$PWD$$/../liel/liel-build-desktop/bin
LIEL_LIB = liel
LIEL_VERSION = 1

BOTAN_HEADERS = $$PWD$$/../3rdparty/botan/build
BOTAN_BUILD = $$PWD$$/../3rdparty/botan-build-desktop/src
win32:BOTAN_BUILD = $$BOTAN_BUILD/release
BOTAN_LIB = Botan

QSA_HEADERS = $$PWD$$/../3rdparty/qtsingleapplication/src
QSA_BUILD = $$PWD$$/../3rdparty/qtsingleapplication/lib
QSA_LIB = QtSolutions_SingleApplication-2.6

# -------------------------------------------------
# Useful things; move elsewhere in future
# -------------------------------------------------

defineReplace(platformversion) {
    libname = $$1
    version = $$2

    win32 {
        message("Building on Windows...")
        return($${libname}$${version})
    } else:macx {
        message("Building on Mac OS X...")
        return($${libname}.$${version})
    } else:linux-g++ {
        message("Building on Linux with GCC...")
        return($${libname})
    } else {
        error("Unknown platform in platformversion!")
    }
}

defineReplace(formatpath) {
    path = $$1

    win32 {
        return(\"$$replace(path, "/", "\\")\")
    } else:unix {
        return($$replace(path, " ", "\\ "))
    } else {
        error("Unknown platform in formatpath!")
    }
}

# Define commands for each platform
win32:COPY_CMD = copy
unix:COPY_CMD = cp -P

win32:DELETE_CMD = del
unix:DELETE_CMD = rm

win32:CMD_SEP = "&"
unix:CMD_SEP = ";"

win32:LIB_EXT = dll
unix:LIB_EXT = so
macx:LIB_EXT = dylib
