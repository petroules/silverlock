echo "This script configures 3rd party code for building with Petroules applications."
echo "Copyright (c) 2010-2011 Petroules Corporation"

# Detect operating system by first assuming we are 
if [ $(uname -o) == "Msys" ]; then # we are Windows (MSYS)
	BUILDDIR="temp-win32-gcc"
fi

if [ $(uname) == "Darwin" ]; then # we are Mac OS X
	BUILDDIR="temp-mac64"
fi

if [ $(uname) == "Linux" ]; then # we are some sort of Linux
	# We only support 32-bit Linux at this point
	BUILDDIR="temp-linux32"
fi

if [ $BUILDDIR == "" ]; then # unknown operating system, error
	echo Unknown operating system $(uname), quitting
	exit
fi

rm -rf "$BUILDDIR"

if [ "$1" == "--clean" ]; then
	exit
fi

mkdir "$BUILDDIR"

# Set filenames
BOTAN_FN=Botan-1.9.12
QTSLNS_FN=qt-solutions-qt-solutions
QTSLNS_AFN=$QTSLNS_FN-master
LIEL_FN=liel-liel
LIEL_AFN=$LIEL_FN-master

# Set directory locations
ARCHIVES_DIR=$PWD/archives
PETROULES_DIR=$PWD/petroules
OUT_DIR=$PWD/$BUILDDIR

echo Extracting Botan source...
tar xzf $ARCHIVES_DIR/$BOTAN_FN.tgz $BOTAN_FN
mv $BOTAN_FN $OUT_DIR/botan

echo Extracting Qt Solutions source...
tar xzf $ARCHIVES_DIR/$QTSLNS_AFN.tar.gz $QTSLNS_FN
mv $QTSLNS_FN $OUT_DIR/qtsolutions

echo Extracting LIEL source...
tar xzf $ARCHIVES_DIR/$LIEL_AFN.tar.gz $LIEL_FN
mv $LIEL_FN $OUT_DIR/liel

echo Extracting create-dmg tool...
unzip $ARCHIVES_DIR/create-dmg.zip -d $OUT_DIR

echo Copying modified QtSingleApplication files...
cp $PETROULES_DIR/qtsingleapplication.cpp $OUT_DIR/qtsolutions/qtsingleapplication/src
cp $PETROULES_DIR/qtsingleapplication.h $OUT_DIR/qtsolutions/qtsingleapplication/src
cp $PETROULES_DIR/botan.sh $OUT_DIR/botan
