import qbs
import qbs.BundleTools
import qbs.File
import qbs.FileInfo

CppApplication {
    Depends { name: "lib-qt" }
    Depends { name: "Qt"; submodules: ["core", "gui", "network", "svg", "xml"] }

    targetName: qbs.targetOS.contains("darwin") ? "Silverlock" : "silverlock"
    version: project.version

    cpp.includePaths: [
        "../..",
        ".",
        "dialogs",
        "widgets"
    ]

    cpp.defines: [ "SILVERLOCKLIB_IMPORTS" ]
    cpp.infoPlistFile: "Info.plist"

    files: [
        "databaseprinter.cpp",
        "databaseprinter.h",
        "databaseprinterfields.cpp",
        "databaseprinterfields.h",
        "documentstate.cpp",
        "documentstate.h",
        "inactivityeventfilter.cpp",
        "inactivityeventfilter.h",
        "Info.plist",
        "main.cpp",
        "mainwindow_menuhandlers.cpp",
        "mainwindow.cpp",
        "mainwindow.h",
        "mainwindow.ui",
        "qversion.cpp",
        "qversion.h",
        "resources.qrc",
        "../../res/globalresources.qrc",
        "silverlock.manifest",
        "silverlock.rc",
        "silverlockapplication.cpp",
        "silverlockapplication.h",
        "silverlockpreferences_keys.h",
        "silverlockpreferences.cpp",
        "silverlockpreferences.h",
        "version.h"
    ]

    Group {
        name: "launcher"
        files: ["silverlock.sh"]
        qbs.install: true
        qbs.installDir: "bin"
    }

    Group {
        name: "freedesktop"
        files: [
            "silverlock.desktop"
        ]

        qbs.install: true
        qbs.installDir: "share/applications"
    }

    Group {
        name: "dialogs"
        prefix: "dialogs/"
        files: [
            "aboutdialog.cpp",
            "aboutdialog.h",
            "aboutdialog.ui",
            "configurecolumnsdialog.cpp",
            "configurecolumnsdialog.h",
            "configurecolumnsdialog.ui",
            "databaseauthenticationdialog.cpp",
            "databaseauthenticationdialog.h",
            "databaseauthenticationdialog.ui",
            "databaseprintpreviewdialog.cpp",
            "databaseprintpreviewdialog.h",
            "databaseprintpreviewdialog.ui",
            "entryeditdialog_helper.cpp",
            "entryeditdialog.cpp",
            "entryeditdialog.h",
            "entryeditdialog.ui",
            "groupeditdialog.cpp",
            "groupeditdialog.h",
            "groupeditdialog.ui",
            "guardeddialog.cpp",
            "guardeddialog.h",
            "licensedialog.cpp",
            "licensedialog.h",
            "licensedialog.ui",
            "nativedialogs.cpp",
            "nativedialogs.h",
            "nativefiledialog.cpp",
            "nativefiledialog.h",
            "nativeopendirectorydialog.cpp",
            "nativeopendirectorydialog.h",
            "nativeopenfiledialog.cpp",
            "nativeopenfiledialog.h",
            "nativeprintdialog.cpp",
            "nativeprintdialog.h",
            "nativesavefiledialog.cpp",
            "nativesavefiledialog.h",
            "nativesystemdialog.cpp",
            "nativesystemdialog.h",
            "newdatabasewizard.cpp",
            "newdatabasewizard.h",
            "newdatabasewizard.ui",
            "preferencesdialog.cpp",
            "preferencesdialog.h",
            "preferencesdialog.ui",
            "searchdatabasedialog.cpp",
            "searchdatabasedialog.h",
            "searchdatabasedialog.ui",
            "selectgroupdialog.cpp",
            "selectgroupdialog.h",
            "selectgroupdialog.ui",
            "updatedialog.cpp",
            "updatedialog.h",
            "updatedialog.ui",
            "welcomedialog.cpp",
            "welcomedialog.h",
            "welcomedialog.ui"
        ]
    }

    Group {
        name: "resources"
        prefix: "res/"
        files: [
            "db.svg",
            "help.png",
            "iexplore.png",
            "print_preview.png",
            "printer.svg"
        ]
    }

    Group {
        name: "translations"
        prefix: "tr/"
        files: [
            "silverlock_de.ts",
            "silverlock_fr.ts"
        ]
    }

    Group {
        name: "widgets"
        prefix: "widgets/"
        files: [
            "entrytablewidget.cpp",
            "entrytablewidget.h",
            "entrytablewidget.ui",
            "entryviewindexes.h",
            "expandingspacerwidget.cpp",
            "expandingspacerwidget.h",
            "groupbrowserwidget.cpp",
            "groupbrowserwidget.h",
            "groupbrowserwidget.ui",
            "toolbarsearchwidget.cpp",
            "toolbarsearchwidget.h",
            "toolbarsearchwidget.ui"
        ]
    }

    property string installDir: type.contains("applicationbundle") ? "Applications" : (qbs.targetOS.contains("windows") ? "." : "bin")
    Group {
        fileTagsFilter: product.type
        qbs.install: true
        qbs.installDir: installDir
    }

    Group {
        fileTagsFilter: ["qm"]
        qbs.install: true
        qbs.installDir: "share/silverlock/translations"
    }

    // QBS-451
    Group {
        condition: qbs.targetOS.contains("osx")
        files: ["../../res/app.icns"]
        qbs.install: true
        qbs.installDir: condition ? FileInfo.joinPaths(product.installDir, BundleTools.unlocalizedResourcesFolderPath(product)) : undefined
    }

    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.defines: ["WINVER=0x0500"]
        cpp.dynamicLibraries: ["user32"]
    }

    Properties {
        condition: qbs.targetOS.contains("osx")
        cpp.frameworks: ["Cocoa"]
    }
}
