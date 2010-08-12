#!/bin/sh

echo
echo "This script creates a DMG file containing an application bundle. It assumes you have already built the application bundle."
echo
echo "What is the UNIX name of the application? "
read userinput
APPNAME=$userinput
echo "What is the friendly/display name of the application? This should also be the name of the bundle minus the .app extension... "
read userinput
FRIENDLYNAME=$userinput
echo "What is the directory in which the application bundle resides?"
read userinput
DIRECTORY=$userinput

BUNDLE="$FRIENDLYNAME.app"
DISKIMAGE="$APPNAME.dmg"
DMGDIR="$PWD/../../../silverlock-build-desktop/bin"

if [ ! -d "$BUNDLE" ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" in directory \"$DIRECTORY\""
    exit
fi

rm "$DMGDIR/$DISKIMAGE"

# Create a symlink to /Applications in the build directory
ln -s "/Applications" "$DMGDIR"

# Create disk image
# The window must be 46 pixels taller than the background image
"$PWD/../../../3rdparty/create-dmg/create-dmg"
    --volname "$FRIENDLYNAME" \
    --background "$PWD/../../../petroules/qt/macx/dmg-background.png" \
    --window-size 600 396 \
    --icon-size 128 \
    --icon "Applications" 160 100 \
    --icon "$BUNDLE" 440 100 \
    "$DISKIMAGE" \
    "$DMGDIR"

hdiutil internet-enable -yes "$DISKIMAGE"

# Move the disk image to the output directory
mv "$DISKIMAGE" "$DMGDIR/$DISKIMAGE"

# Remove Applications folder link
rm "$DMGDIR/Applications"
