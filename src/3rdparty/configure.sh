echo "This script configures 3rd party code for building with Petroules applications."
echo "Copyright (c) 2010-2011 Petroules Corporation"
echo ""

# Set physical directory of shell script
pushd $(dirname $0)

# Detect operating system
if [ $(uname) == "Linux" ]; then # we are some sort of Linux
	# We only support 32-bit Linux at this point
	BUILDDIR="$PWD/temp-linux32"
fi

if [ $(uname) == "Darwin" ]; then # we are Mac OS X
	BUILDDIR="$PWD/temp-mac64"
fi

if [ $(uname -o) == "Msys" ]; then # we are Windows (MSYS)
	BUILDDIR="$PWD/temp-win32-gcc"
fi

if [ $BUILDDIR == "" ]; then # unknown operating system, error
	echo Unknown operating system $(uname), quitting
	exit
fi

# Clean previous make file and temp directory
if [ -f "$PWD/make.sh" ]; then
	rm "$PWD/make.sh"
fi

if [ -d "$BUILDDIR" ]; then
	rm -rf "$BUILDDIR"
fi

if [ "$1" == "--clean" ]; then
	echo "Removed temporary files"
	exit
fi

mkdir "$BUILDDIR"

# Set filenames
BOTAN_FN=Botan-1.10.1
QTSLNS_FN=qt-solutions-qt-solutions
QTSLNS_AFN=$QTSLNS_FN-master
SYNTEZA_FN=synteza-synteza
SYNTEZA_AFN=$SYNTEZA_FN-master

# Set directory locations
ARCHIVES_DIR=$PWD/archives
PETROULES_DIR=$PWD/petroules
OUT_DIR=$BUILDDIR

echo Extracting Botan source...
tar xzf $ARCHIVES_DIR/$BOTAN_FN.tgz $BOTAN_FN
mv $BOTAN_FN $OUT_DIR/botan

echo Extracting Qt Solutions source...
tar xzf $ARCHIVES_DIR/$QTSLNS_AFN.tar.gz $QTSLNS_FN
mv $QTSLNS_FN $OUT_DIR/qtsolutions

echo Extracting Synteza source...
tar xzf $ARCHIVES_DIR/$SYNTEZA_AFN.tar.gz $SYNTEZA_FN
mv $SYNTEZA_FN $OUT_DIR/synteza

# Only need create-dmg for Mac OS X
if [ $(uname) == "Darwin" ]; then
	echo Extracting create-dmg tool...
	unzip $ARCHIVES_DIR/create-dmg.zip -d $OUT_DIR
fi

echo Copying modified QtSingleApplication files...
cp $PETROULES_DIR/qtsingleapplication.cpp $OUT_DIR/qtsolutions/qtsingleapplication/src
cp $PETROULES_DIR/qtsingleapplication.h $OUT_DIR/qtsolutions/qtsingleapplication/src
cp $PETROULES_DIR/botan.sh $OUT_DIR/botan
cp $PETROULES_DIR/android.txt $OUT_DIR/botan/src/build-data/os

echo Creating make script...
echo $BUILDDIR/botan/botan.sh > "$PWD/make.sh"

popd
