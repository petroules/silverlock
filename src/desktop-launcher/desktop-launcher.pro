# --------------------------------------------------
# This section contains project configuration
# directives such as the required Qt modules, the
# project template, and any variables that need to
# be defined.
# --------------------------------------------------

QT -= gui
QT += core network
TEMPLATE = app
TARGET = silverlock-launcher

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

HEADERS += windowscertificatevalidator.h
SOURCES += windowscertificatevalidator.cpp main.cpp

# --------------------------------------------------
# This section contains all libraries that the
# project links to, first system libraries, and then
# libraries in the project build tree.
# --------------------------------------------------

LIBS += -lcrypt32

# --------------------------------------------------
# This section contains miscellaneous commands such
# as Windows resource files and icons for Mac OS X
# --------------------------------------------------

RC_FILE = ../desktop/silverlock.rc
