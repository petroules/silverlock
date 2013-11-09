Project {
    property string version: [ versionMajor, versionMinor, versionPatch ].join(".")

    property int versionMajor: 1
    property int versionMinor: 2
    property int versionPatch: 0

    property bool buildDistributionPackages: false

    qbsSearchPaths: "qbs"

    references: [
        "package/osx/appstore-pkg.qbs",
        "package/osx/dmg.qbs",
        "src/3rdparty/botan.qbs",
        "src/app/app.qbs",
        "src/lib/qt/lib-qt.qbs"
    ]
}
