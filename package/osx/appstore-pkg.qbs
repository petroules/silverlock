import qbs
import qbs.FileInfo
import qbs.Process

Product {
    Depends { name: "app" }
    type: ["pkg"]

    condition: qbs.targetOS.contains("osx") && project.buildDistributionPackages

    name: "appstore-pkg"
    targetName: "silverlock-" + project.version + "-osx-" + qbs.architecture

    files: ["macdeployqt"]

    Rule {
        multiplex: true
        inputs: ["qbs"]
        usings: ["applicationbundle"]

        Artifact {
            fileName: FileInfo.joinPaths(product.destinationDirectory, product.targetName + ".pkg")
            fileTags: ["pkg"]
        }

        prepare: {
            var cmd;
            var cmds = [];
            var applicationBundle = inputs.applicationbundle[0].fileName;
            var pkgroot = FileInfo.joinPaths(product.buildDirectory, "GeneratedFiles", product.name);

            cmd = new Command("rm", ["-rf", pkgroot]);
            cmd.silent = true;
            cmds.push(cmd);

            cmd = new Command("mkdir", ["-p", pkgroot]);
            cmd.silent = true;
            cmds.push(cmd);

            cmd = new Command("cp", ["-R", applicationBundle, pkgroot]);
            cmd.silent = true;
            cmds.push(cmd);

            cmd = new JavaScriptCommand();
            cmd.description = "macdeployqt " + FileInfo.fileName(applicationBundle);
            cmd.pkgroot = pkgroot;
            cmd.applicationBundle = applicationBundle;
            cmd.sourceCode = function() {
                var process = new Process();
                try {
                    process.setWorkingDirectory(pkgroot);
                    process.setEnv("QT_INSTALL_LIBS", inputs.applicationbundle[0].moduleProperty("Qt/core", "libPath"));
                    process.exec(FileInfo.joinPaths(product.sourceDirectory, "macdeployqt"),
                                 [FileInfo.fileName(applicationBundle)]);
                    if (process.exitCode() !== 0)
                        throw(process.readStdErr());
                } finally {
                    process.close();
                }
            };
            cmds.push(cmd);

            cmd = new JavaScriptCommand();
            cmd.description = "signing " + FileInfo.fileName(applicationBundle);
            cmd.pkgroot = pkgroot;
            cmd.applicationBundle = applicationBundle;
            cmd.sourceCode = function() {
                var process, codesignAllocate;

                process = new Process();
                try {
                    process.exec("xcrun", ["--sdk", "macosx", "--find", "codesign_allocate"], true);
                    codesignAllocate = process.readStdOut().trim();
                } finally {
                    process.close();
                }

                process = new Process();
                try {
                    process.setWorkingDirectory(pkgroot);
                    process.setEnv("CODESIGN_ALLOCATE", codesignAllocate);
                    process.exec("codesign", ["--deep", "--verbose", "--force", "--sign",
                                    "3rd Party Mac Developer Application:", FileInfo.fileName(applicationBundle)]);
                    if (process.exitCode() !== 0)
                        throw(process.readStdErr());
                } finally {
                    process.close();
                }
            };
            cmds.push(cmd);

            cmd = new Command("productbuild", ["--component", FileInfo.fileName(applicationBundle),
                                               "/Applications", "--sign",
                                               "3rd Party Mac Developer Installer:",
                                               output.fileName]);
            cmd.description = "creating " + FileInfo.fileName(output.fileName);
            cmd.workingDirectory = pkgroot;
            cmd.stdoutFilterFunction = function(output) {
                return output.replace(/productbuild: (.*)\n/g, "");
            };
            cmds.push(cmd);

            cmd = new Command("rm", ["-rf", pkgroot]);
            cmd.silent = true;
            cmds.push(cmd);

            return cmds;
        }
    }
}

