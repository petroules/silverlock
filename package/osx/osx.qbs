import qbs
import qbs.FileInfo
import qbs.Process

Project {
    Product {
        Depends { name: "app" }
        type: ["pkg"]

        condition: qbs.targetOS.contains("osx")

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

    Product {
        Depends { name: "app" }
        type: ["dmg"]

        condition: qbs.targetOS.contains("osx")

        name: "dmg"
        targetName: "silverlock-" + project.version + "-osx-" + qbs.architecture

        files: ["dmg-background.png", "DS_Store", "macdeployqt"]

        Rule {
            multiplex: true
            inputs: ["qbs"]
            usings: ["applicationbundle"]

            Artifact {
                fileName: FileInfo.joinPaths(product.destinationDirectory, product.targetName + ".dmg")
                fileTags: ["dmg"]
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
                                        "Developer ID Application:", FileInfo.fileName(applicationBundle)]);
                        if (process.exitCode() !== 0)
                            throw(process.readStdErr());
                    } finally {
                        process.close();
                    }
                };
                cmds.push(cmd);

                cmd = new Command("ln", ["-s", "/Applications", "Applications"]);
                cmd.workingDirectory = pkgroot;
                cmd.silent = true;
                cmds.push(cmd);

                cmd = new Command("cp", [FileInfo.joinPaths(product.sourceDirectory, "DS_Store"),
                                         FileInfo.joinPaths(pkgroot, ".DS_Store")]);
                cmd.silent = true;
                cmds.push(cmd);

                cmd = new Command("mkdir", ["-p", FileInfo.joinPaths(pkgroot, ".background")]);
                cmd.silent = true;
                cmds.push(cmd);

                // Background images should be TIFFs at 72 DPI
                var bgSource = FileInfo.joinPaths(product.sourceDirectory, "dmg-background.png");
                var bgDest = FileInfo.joinPaths(pkgroot, ".background", "backgroundImage.tiff");
                cmd = new Command("sips", ["-s", "format", "tiff",
                                           "-s", "formatOptions", "normal",
                                           bgSource,
                                           "-s", "dpiWidth", 72,
                                           "-s", "dpiHeight", 72,
                                           "--out", bgDest]);
                cmd.silent = true;
                cmd.stdoutFilterFunction = "function(output) {";
                cmd.stdoutFilterFunction += "output = output.replace(\"" + bgSource.replace("\"", "\\\"") + "\\n\", '');";
                cmd.stdoutFilterFunction += "output = output.replace(\"  " + bgDest.replace("\"", "\\\"") + "\\n\", '');";
                cmd.stdoutFilterFunction += "return output;";
                cmd.stdoutFilterFunction += "};";
                cmds.push(cmd);

                cmd = new Command("hdiutil", ["create", output.fileName,
                                              "-srcfolder", ".",
                                              "-format", "UDZO",
                                              "-volname", FileInfo.completeBaseName(applicationBundle), "-ov"]);
                cmd.description = "creating " + FileInfo.fileName(output.fileName);
                cmd.workingDirectory = pkgroot;
                cmd.stdoutFilterFunction = function(output) {
                    return output.replace(/created: \/(.*)\.dmg\n/, "");
                };
                cmds.push(cmd);

                cmd = new Command("hdiutil", ["internet-enable", "-yes", output.fileName]);
                cmd.silent = true;
                cmd.stderrFilterFunction = function(output) {
                    return output.replace("hdiutil: internet-enable: enable succeeded\n", "");
                };
                cmds.push(cmd);

                cmd = new Command("rm", ["-rf", pkgroot]);
                cmd.silent = true;
                cmds.push(cmd);

                return cmds;
            }
        }
    }
}
