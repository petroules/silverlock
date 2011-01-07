echo "This script configures 3rd party code for building with Petroules applications."
echo "Copyright (c) 2010-2011 Petroules"

rm -rf "temp"

if [ "$1" == "--clean" ]; then
	exit
fi

mkdir "temp"

# Set filenames
BOTAN_FN=Botan-1.9.12
QTSLNS_FN=qt-solutions-qt-solutions
QTSLNS_AFN=$QTSLNS_FN-master
LIEL_FN=liel-liel
LIEL_AFN=$LIEL_FN-master

# Set directory locations
ARCHIVES_DIR=$PWD/archives
PETROULES_DIR=$PWD/petroules
OUT_DIR=$PWD/temp

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
