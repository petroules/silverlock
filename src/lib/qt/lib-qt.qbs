import qbs

DynamicLibrary {
    Depends { name: "cpp" }
    Depends { name: "botan" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.xml" }

    targetName: "silvercore"
    version: "1.0.0"
    destinationDirectory: "lib"

    cpp.defines: [ "SILVERLOCKLIB_EXPORTS" ]
    cpp.includePaths: [ "." ]

    files: [
        "database_keys.h",
        "database.cpp",
        "database.h",
        "databasecrypto.cpp",
        "databasecrypto.h",
        "databasenode.cpp",
        "databasenode.h",
        "databasereader.cpp",
        "databasereader.h",
        "databasewriter.cpp",
        "databasewriter.h",
        "entry.cpp",
        "entry.h",
        "group.cpp",
        "group.h",
        "searchparameters.cpp",
        "searchparameters.h",
        "silverlocklib_global.h",
        "silverlocklib.h",
        "silverlocklib.rc",
        "version.h"
    ]

    Group {
        name: "translations"
        prefix: "tr/"
        files: [
            "silverlocklib_de.ts",
            "silverlocklib_fr.ts"
        ]
    }

    Group {
        fileTagsFilter: product.type.concat(["dynamiclibrary_symlink"])
        qbs.install: true
        qbs.installDir: product.destinationDirectory
    }

    Properties {
        condition: qbs.targetOS.contains("windows")
        cpp.defines: [ "NOMINMAX", "WINVER=0x0500" ]
        cpp.dynamicLibraries: [ "advapi32", "user32" ] // for Botan
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [ "." ]
    }
}
