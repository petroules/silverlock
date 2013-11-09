Project {
    property string version: "1.1.2"

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
