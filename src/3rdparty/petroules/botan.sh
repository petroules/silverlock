#!/bin/sh

echo "This script builds Botan using GCC+MSYS on Windows, and GCC on Unix."
echo "This file is in the public domain."

CPU_TYPE="x86"
MINGW_OS=
DEBUG_BUILD=
TARGET_SUFFIX=

# If we're on Mac OS X, use 64-bit
if [ $(uname) == "Darwin" ]; then
	CPU_TYPE="x64"
fi

# If we're compiling for Windows using MSYS, tell Botan such so it compiles properly
if [ $(uname -o) == "Msys" ]; then
	MINGW_OS="--os=mingw"
	
	# If the first argument to the script was --debug, then we need to rename the resulting library to botand instead of botan
	if [ $0 == "--debug" ]; then
		TARGET_SUFFIX="d"
	fi
fi

# If we want to build in debug mode, then enable debug mode...
if [ $0 == "--debug" ]; then
	DEBUG_BUILD="--enable-debug"
fi

# Configure Botan using its Python configure script
# Here we want to make sure to select x86 processor as building 64-bit Qt-based Petroules apps is not yet supported except on Mac OS X since Cocoa is built as 64-bit by default and all new Macs use 64-bit processors
./configure.py $MINGW_OS --cpu=$CPU_TYPE --disable-shared --disable-asm $DEBUG_BUILD

# Build Botan and check it
make
make check

# If we're on Windows, name the static library correctly - Windows does not prefix with "lib", uses the .lib file extension, and adds a "d" suffix if the library was built in debug mode.
if [ -f "/msys.bat" ]; then
	mv libbotan.a botan$TARGET_SUFFIX.lib
fi
