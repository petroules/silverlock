#!/bin/sh

echo
echo "This script creates a DMG file containing an application bundle. It assumes you have already built the application bundle."
echo

APPNAME="silverlock"
FRIENDLYNAME="Silverlock"

BUNDLE="$FRIENDLYNAME.app"
DISKIMAGE="$APPNAME.dmg"
DMGDIR="$PWD/../../../silverlock-build-desktop/bin"

if [ ! -d "$DMGDIR/$BUNDLE" ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" in directory \"$DIRECTORY\""
    exit
fi

if [ -f "$DMGDIR/$DISKIMAGE" ] ; then
    rm "$DMGDIR/$DISKIMAGE"
fi

# Create a symlink to /Applications in the build directory
ln -s "/Applications" "$DMGDIR"

# Create disk image
# The window must be 46 pixels taller than the background image
"$PWD/../../../3rdparty/create-dmg/create-dmg" \
    --volname "$FRIENDLYNAME" \
    --background "$PWD/../../../petroules/qt/macx/dmg-background.png" \
    --window-size 600 396 \
    --icon-size 128 \
	--icon "$BUNDLE" 160 100 \
    --icon "Applications" 440 100 \
    "$DISKIMAGE" \
    "$DMGDIR"

hdiutil internet-enable -yes "$DISKIMAGE"

# Move the disk image to the output directory
mv "$DISKIMAGE" "$DMGDIR/$DISKIMAGE"

# Remove Applications folder link
rm "$DMGDIR/Applications"
