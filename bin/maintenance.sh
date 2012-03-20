#!/bin/bash

. ./functions.sh

if [ ! `which git` ] ; then
	echo "FATAL: This script requires git - install it from http://git-scm.org"
	exit 1
fi

DEP_DIR=../lib
DEP_PETROULES_UTILITIES_DIR=$DEP_DIR/petroules-utilities-qt
DEP_BOTAN_DIR=../src/libqt/botan/botan
SHARED_OPTS="--disable-shared --disable-asm --gen-amalgamation $BOTAN_DEBUG"

# Gets the latest Petroules Utilities from Git
function update-petroules-utilities()
{
	# Remove existing Synteza directory
	if [ -d "$DEP_PETROULES_UTILITIES_DIR" ] ; then
		rm -rf "$DEP_PETROULES_UTILITIES_DIR"
	fi

	cecho "Fetching Petroules Utilities..." blue bold

	# Download latest Petroules Utilities from Git
	pushd `dirname "$DEP_PETROULES_UTILITIES_DIR"` >/dev/null
	git clone git://github.com/petroules/petroules-utilities-qt.git
	popd >/dev/null
}

# Appends the specified text to both the botan_all source file and header
function bappend()
{
	echo -e "$1" >> "$DEP_BOTAN_DIR/../botan_all.cpp"
	echo -e "$1" >> "$DEP_BOTAN_DIR/../botan_all.h"
}

# Gets the latest Botan from Git
function gen-botan()
{
	local COND=$1
	local OS=$2
	local ARCH=$3
	local OPTS=$4

	cecho "Configuring Botan for $OS ($ARCH)..." blue bold

	pushd "$DEP_BOTAN_DIR" >/dev/null
	./configure.py --os=$OS --cpu=$ARCH $SHARED_OPTS $OPTS >/dev/null
	popd >/dev/null

	bappend "#elif $COND"
	cat "$DEP_BOTAN_DIR/botan_all.cpp" >> "$DEP_BOTAN_DIR/../botan_all.cpp"
	cat "$DEP_BOTAN_DIR/botan_all.h" >> "$DEP_BOTAN_DIR/../botan_all.h"

	if [ "$OS" = "android" ] ; then
		# Stop eclipse from whining about missing definitions because it is incompetent
		# Also, -E is for BSD/OSX... GNU/Linux use -r
		if [ "`uname -s`" = "Linux" ] ; then
			SED_OPT="-r" # GNU/Linux
		else
			SED_OPT="-E" # BSD/OSX
		fi

		sed $SED_OPT -i '' 's/std(::(memset|memcpy|memmove))/\1/g' "$DEP_BOTAN_DIR/botan_all.h"

		cp "$DEP_BOTAN_DIR"/botan_all.* "../src/android/jni"
	fi

	pushd "$DEP_BOTAN_DIR" >/dev/null
	make clean
	make distclean
	popd >/dev/null
}

function update-botan()
{
	# Download Botan
	if [ ! -d "$DEP_BOTAN_DIR" ] ; then
		cecho "Fetching Botan..." blue bold
		pushd `dirname "$DEP_BOTAN_DIR"` >/dev/null
		git clone git://github.com/petroules/botan.git
		popd >/dev/null
	fi

	# Add android build file
	echo -e "os_type unix\n\
	\n\
	<target_features>\n\
	gettimeofday\n\
	posix_mlock\n\
	gmtime_r\n\
	dlopen\n\
	</target_features>\n\
	" > "$DEP_BOTAN_DIR/src/build-data/os/android.txt"

	rm "$DEP_BOTAN_DIR/../botan_all.cpp"
	rm "$DEP_BOTAN_DIR/../botan_all.h"

	bappend "#ifdef QT_CORE_LIB"
	bappend "#include <QtCore>"
	bappend "#endif"
	bappend "#if 0"

	gen-botan "defined(Q_OS_ANDROID) || defined(__ANDROID__)" android arm "--no-autoload --enable-modules=auto_rng,pbkdf2,dev_random,sha1"
	gen-botan "defined(Q_WS_WIN32) && defined(Q_CC_MINGW)" mingw i686
	gen-botan "defined(Q_WS_WIN32) && defined(Q_CC_MSVC)" windows i686
	gen-botan "defined(Q_WS_WIN64) && defined(Q_CC_MSVC)" windows x86_64
	gen-botan "defined(Q_WS_MAC32)" macosx i686
	gen-botan "defined(Q_WS_MAC64)" macosx x86_64
	gen-botan "defined(Q_OS_LINUX) && !defined(__LP64__) && !defined(_LP64)" linux i686
	gen-botan "defined(Q_OS_LINUX) && (defined(__LP64__) && defined(_LP64))" linux x86_64

	bappend "#else\n#error \"Not a supported platform for building Botan\"\n#endif"

	rm -rf "$DEP_BOTAN_DIR"
}

function get-create-dmg()
{
	# Download YourSway create-dmg if necesary
	DEP_DMG=yoursway-create-dmg
	if [ ! -d "$DEP_DMG" ] ; then
		cecho "Downloading YourSway create-dmg utility..." blue bold
		git clone "git://github.com/andreyvit/$DEP_DMG.git"
	else
		# Check for any updates
		pushd "$DEP_DMG" > /dev/null
		git pull
		popd > /dev/null
	fi
}

if [ "$1" = "--update-dependencies" ] ; then
	update-petroules-utilities
	update-botan
elif [ "$1" = "--remove-dependencies" ] ; then
	rm -rf "$DEP_PETROULES_UTILITIES_DIR"
	rm -rf "$DEP_BOTAN_DIR"
elif [ "$1" = "--update-translations" ] ; then
	mkdir ../src/libqt/tr
	lupdate -no-obsolete ../src/libqt/libqt.pro

	mkdir ../src/desktop/tr
	lupdate -no-obsolete ../src/desktop/desktop.pro
else
	echo "Usage: $0 [OPTION]"
	echo ""
	echo "  --update-dependencies"
	echo "      Updates all 3rd party dependencies to their latest versions"
	echo ""
	echo "  --remove-dependencies"
	echo "      Removes all 3rd party dependencies - will break the build"
	echo ""
	echo "  --update-translations"
	echo "      Updates all Qt translation files"
fi
