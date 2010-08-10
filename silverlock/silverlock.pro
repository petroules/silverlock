# -------------------------------------------------
# Project created by QtCreator 2010-04-08T06:08:50
# -------------------------------------------------
include(../common.pri)
include(version.pri)

QT += network \
    svg \
    xml
TARGET = $$APP_UNIXNAME
TEMPLATE = app
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    entryeditdialog.cpp \
    groupeditdialog.cpp \
    preferencesdialog.cpp \
    silverlockpreferences.cpp \
    groupbrowserwidget.cpp \
    entrytablewidget.cpp \
    selectgroupdialog.cpp \
    searchdatabasedialog.cpp \
    documentstate.cpp \
    configurecolumnsdialog.cpp \
    databaseauthenticationdialog.cpp \
    aboutdialog.cpp \
    licensedialog.cpp \
    updatedialog.cpp \
    guardeddialog.cpp \
    inactivityeventfilter.cpp \
    entryeditdialog_helper.cpp \
    databaseprintdialog.cpp
HEADERS += \
    mainwindow.h \
    entryeditdialog.h \
    groupeditdialog.h \
    preferencesdialog.h \
    silverlockpreferences.h \
    groupbrowserwidget.h \
    entrytablewidget.h \
    silverlockpreferences_keys.h \
    selectgroupdialog.h \
    searchdatabasedialog.h \
    documentstate.h \
    configurecolumnsdialog.h \
    entryviewindexes.h \
    databaseauthenticationdialog.h \
    aboutdialog.h \
    licensedialog.h \
    updatedialog.h \
    guardeddialog.h \
    stable.h \
    inactivityeventfilter.h \
    databaseprintdialog.h \
    version.h
PRECOMPILED_HEADER = stable.h
FORMS += mainwindow.ui \
    entryeditdialog.ui \
    groupeditdialog.ui \
    preferencesdialog.ui \
    groupbrowserwidget.ui \
    entrytablewidget.ui \
    selectgroupdialog.ui \
    searchdatabasedialog.ui \
    configurecolumnsdialog.ui \
    databaseauthenticationdialog.ui \
    aboutdialog.ui \
    licensedialog.ui \
    updatedialog.ui \
    databaseprintdialog.ui
RESOURCES += resources.qrc \
    ../res/globalresources.qrc
TRANSLATIONS += tr/silverlock_de.ts \
    tr/silverlock_fr.ts
DESTDIR = ../bin
INCLUDEPATH += ../silverlocklib $$LIEL_HEADERS $$QSA_HEADERS
LIBS += -L../lib -l$$platformversion(silverlocklib, 1) -L$$LIEL_BUILD -l$$platformversion($$LIEL_LIB, $$LIEL_VERSION) -L$$QSA_BUILD -l$$QSA_LIB
OTHER_FILES += silverlock.rc \
    silverlock.manifest \
    Info.plist \
    version.pri

# Copy over dependent libraries - not necessary on Mac OS X as macdeployqt will take care of it...
!macx {
    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$OUT_PWD/../lib/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$LIEL_BUILD/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$BOTAN_BUILD/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$QSA_BUILD/*.$$LIB_EXT) $$formatpath($$OUT_PWD/$$DESTDIR) $$CMD_SEP
}

win32 {
    RC_FILE = silverlock.rc
}

macx {
    ICON = res/app.icns

    QMAKE_INFO_PLIST = Info.plist

    PLISTDIR = $$OUT_PWD/$$DESTDIR/$${TARGET}.app/Contents
    PLISTFILE = \"$$PLISTDIR/Info.plist\"
    PLISTFILE_BAK = \"$$PLISTDIR/Info.plist-bak\"

    # EXECUTABLE and ICON will be taken care of by QMake
    QMAKE_POST_LINK += $$COPY_CMD $$PLISTFILE $$PLISTFILE_BAK $$CMD_SEP
    QMAKE_POST_LINK += sed \
                        -e \"s,@DISPLAY_NAME@,$${APP_DISPLAYNAME},g\" \
                        -e \"s,@BUNDLE_IDENTIFIER@,$${APP_BUNDLEID},g\" \
                        -e \"s,@VERSION@,$${VER_FILEVERSION_STR},g\" \
                        -e \"s,@SHORT_VERSION@,$${VER_PRODUCTVERSION_STR},g\" \
                        -e \"s,@COPYRIGHT@,$${VER_LEGALCOPYRIGHT_STR},g\" \
                        $$PLISTFILE_BAK > $$PLISTFILE $$CMD_SEP
    QMAKE_POST_LINK += $$DELETE_CMD $$PLISTFILE_BAK $$CMD_SEP

    QMAKE_POST_LINK += macdeployqt $$formatpath($$OUT_PWD/$$DESTDIR/$${TARGET}.app) $$CMD_SEP
}

linux-g++ {
    QMAKE_POST_LINK += $$COPY_CMD $$formatpath($$PWD/../deploy/linux/launcher.sh) $$formatpath($$OUT_PWD/$$DESTDIR/$${TARGET}.sh) $$CMD_SEP
    QMAKE_POST_LINK += chmod +x $$formatpath($${OUT_PWD}/$${DESTDIR}/$${TARGET}.sh) $$CMD_SEP
}
