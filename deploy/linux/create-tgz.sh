BUILD_DIR=$PWD/../../silverlock-build-desktop
TEMP_DIR=$PWD/silverlock
QT_DIR=/usr/local/Trolltech/Qt-4.7.1

# Remove previous temp directory and remake it
rm -rf $TEMP_DIR
mkdir $TEMP_DIR

# Copy over launcher script and desktop entry file
cp silverlock.* $TEMP_DIR

# Copy over Silverlock executable and libraries
cp $BUILD_DIR/desktop/silverlock $TEMP_DIR
cp -R $BUILD_DIR/libqt/libsilverlocklib.* $TEMP_DIR
cp -R $BUILD_DIR/3rdparty/temp/liel/qt/liel/libliel.* $TEMP_DIR

# Give Silverlock and its launcher the executable bit
chmod +x $TEMP_DIR/silverlock
chmod +x $TEMP_DIR/silverlock.sh

# Copy over Qt libraries
cp -R $QT_DIR/lib/libQtCore.so* $TEMP_DIR
cp -R $QT_DIR/lib/libQtGui.so* $TEMP_DIR
cp -R $QT_DIR/lib/libQtNetwork.so* $TEMP_DIR
cp -R $QT_DIR/lib/libQtSvg.so* $TEMP_DIR
cp -R $QT_DIR/lib/libQtXml.so* $TEMP_DIR
cp -R $QT_DIR/lib/libQtWebKit.so* $TEMP_DIR

# Copy over Qt plugins
mkdir $TEMP_DIR/accessible
cp $QT_DIR/plugins/accessible/*.so $TEMP_DIR/accessible

mkdir $TEMP_DIR/imageformats
cp $QT_DIR/plugins/imageformats/*.so $TEMP_DIR/imageformats

if [ -f $TEMP_DIR/libQtSvg.so ]; then
	mkdir $TEMP_DIR/iconengines
	cp $QT_DIR/plugins/iconengines/*.so $TEMP_DIR/iconengines
fi

# Create the gzipped tarball and then remove the temp directory
tar czf silverlock.tar.gz silverlock
rm -rf $TEMP_DIR
