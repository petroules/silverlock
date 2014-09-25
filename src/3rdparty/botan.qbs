import qbs
import qbs.FileInfo

StaticLibrary {
    Depends { name: "cpp" }

    name: "botan"
    version: "1.10"

    property path botanSourceDirectory: "botan"
    property string makeTool: qbs.toolchain.contains("msvc") ? "nmake" : "make"

    Rule {
        inputs: [ "qbs" ]

        Artifact {
            filePath: FileInfo.joinPaths(product.destinationDirectory, "botan",
                        product.moduleProperty("cpp", "staticLibraryPrefix") +
                        product.targetName + "-" + product.version +
                        product.moduleProperty("cpp", "staticLibrarySuffix"))
            fileTags: [ "staticlibrary" ]
        }

        prepare: {
            var i;
            var cmd;
            var cmds = [];
            var args = [];
            var botanConfigureScript = FileInfo.joinPaths(product.botanSourceDirectory, "configure.py");
            var botanBuildDirectory = FileInfo.path(output.filePath);

            if (product.moduleProperty("qbs", "hostOS").contains("windows"))
                cmd = new Command("cmd", [ "/c", "mkdir", botanBuildDirectory ]);
            else
                cmd = new Command("mkdir", [ "-p", botanBuildDirectory ]);
            cmd.silent = true;
            cmds.push(cmd);

            args = [ botanConfigureScript ];
            if (product.moduleProperty("cpp", "debugInformation"))
                args.push("--enable-debug");

            args.push("--cc=" + product.moduleProperty("qbs", "toolchain")[0]);
            args.push("--cpu=" + product.moduleProperty("qbs", "architecture"));
            if (product.moduleProperty("qbs", "toolchain").contains("mingw"))
                args.push("--os=mingw");
            else if (product.moduleProperty("qbs", "targetOS").contains("darwin"))
                args.push("--os=darwin");

            args.push("--disable-shared");
            args.push("--disable-asm");
            args.push("--gen-amalgamation");
            args.push("--with-build-dir=" + botanBuildDirectory);

            args.push("--no-autoload");
            args.push("--enable-modules=auto_rng,pbkdf2,dev_random,sha1,aes,cbc");

            cmd = new Command("python", args);
            cmd.description = "configuring botan";
            cmd.workingDirectory = botanBuildDirectory;
            cmds.push(cmd);

            cmd = new Command(product.makeTool, product.makeTool === "nmake" ? ["/S"] : ["-s"]);
            cmd.description = "making botan";
            cmd.workingDirectory = botanBuildDirectory;
            cmds.push(cmd);

            return cmds;
        }
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: FileInfo.joinPaths(product.buildDirectory)
    }
}
