TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
    3rdparty/temp/liel/qt/liel \
    libqt \
    desktop
win32:!wince*:SUBDIRS += desktop-launcher
