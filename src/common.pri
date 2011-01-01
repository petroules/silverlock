include($$PWD/../common/petroules/qt/petroules-common.pri)

# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500
