BUILD_DIR=$PWD/../../silverlock-build-desktop

rm -rf $PWD/temp
mkdir $PWD/temp
cp silverlock.* $PWD/temp
cp $BUILD_DIR/desktop/silverlock $PWD/temp
cp $BUILD_DIR/libqt/libsilverlocklib.* $PWD/temp
cp $BUILD_DIR/3rdparty/temp/liel/qt/liel/libliel.* $PWD/temp

tar czf silverlock.tar.gz $PWD/temp