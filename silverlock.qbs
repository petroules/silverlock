Project {
    property string version: "1.1.2"

    property bool buildDistributionPackages: false

    qbsSearchPaths: "qbs"

    references: [
        "lib/botan.qbs",
        "src/app/app.qbs",
        "src/lib/qt/lib-qt.qbs"
    ]

    SubProject {
        condition: project.buildDistributionPackages
        filePath: "package/osx/osx.qbs"
    }
}
