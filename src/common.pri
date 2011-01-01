# We want Windows methods from at least Windows 2000 (NT 5.0)
win32:DEFINES += WINVER=0x0500

defineReplace(populateplist) {
    PLISTDIR = $$OUT_PWD/$$DESTDIR/$${TARGET}.app/Contents
    PLISTFILE = $$PLISTDIR/Info.plist
    PLISTFILE_BAK = $$PLISTDIR/Info.plist-bak

    # EXECUTABLE and ICON will be taken care of by QMake
    PLIST_COMMANDS += cp -R $$PLISTFILE $$PLISTFILE_BAK ;
    PLIST_COMMANDS += sed \
                        -e \"s,@DISPLAY_NAME@,$${APP_DISPLAYNAME},g\" \
                        -e \"s,@BUNDLE_IDENTIFIER@,$${APP_BUNDLEID},g\" \
                        -e \"s,@VERSION@,$${VER_FILEVERSION_STR},g\" \
                        -e \"s,@SHORT_VERSION@,$${VER_PRODUCTVERSION_STR},g\" \
                        -e \"s,@COPYRIGHT@,$${VER_LEGALCOPYRIGHT_STR},g\" \
                        $$PLISTFILE_BAK > $$PLISTFILE ;
    PLIST_COMMANDS += rm $$PLISTFILE_BAK ;

    return($$PLIST_COMMANDS)
}

defineReplace(renamebundle) {
    return(mv $$OUT_PWD/$$DESTDIR/$${TARGET}.app $$OUT_PWD/$$DESTDIR/$${APP_DISPLAYNAME}.app ;)
}

defineReplace(macdeployqt) {
    VERBOSITY = $$1

    # Delete the previous application bundle's frameworks and plugins so macdeployqt doesn't mess up
    MACDEPLOY_COMMANDS += rm -r $$formatpath($$OUT_PWD/$$DESTDIR/$${APP_DISPLAYNAME}.app/Contents/Frameworks) ;
    MACDEPLOY_COMMANDS += rm -r $$formatpath($$OUT_PWD/$$DESTDIR/$${APP_DISPLAYNAME}.app/Contents/PlugIns) ;
    MACDEPLOY_COMMANDS += rm $$formatpath($$OUT_PWD/$$DESTDIR/$${APP_DISPLAYNAME}.app/Contents/Resources/qt.conf) ;

    # Run macdeployqt
    MACDEPLOY_COMMANDS += macdeployqt $$formatpath($$OUT_PWD/$$DESTDIR/$${TARGET}.app) -verbose=$$VERBOSITY ;

    return($$MACDEPLOY_COMMANDS)
}
