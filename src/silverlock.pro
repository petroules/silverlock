TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    petroules-utilities-qt \
    libqt \
    desktop \
    libqttest

# Additional subprojects for Windows only
win32:!wince*:SUBDIRS += desktop-launcher
