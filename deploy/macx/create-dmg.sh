#!/bin/sh

echo
echo "This script creates a DMG file containing an application bundle. It assumes you have already built the application bundle."
echo

APPNAME="silverlock"
FRIENDLYNAME="Silverlock"

OLDBUNDLE="$APPNAME.app"
BUNDLE="$FRIENDLYNAME.app"
DISKIMAGE="$APPNAME.dmg"
SRCDIR="$PWD/../../$APPNAME-build-desktop/desktop"
DMGDIR="$PWD/tmp"
CREATEDMGDIR="$PWD/../../src/3rdparty/temp/create-dmg"

if [ ! -d "$SRCDIR/$OLDBUNDLE" ] ; then
    echo "ERROR: cannot find application bundle \"$OLDBUNDLE\" in directory \"$SRCDIR\""
    exit
fi

# Remove previous disk image
if [ -f "$DISKIMAGE" ] ; then
    rm "$DISKIMAGE"
fi

# Copy the bundle from the build directory, macdeployqt it, and rename it
mkdir "$DMGDIR"
cp -R "$SRCDIR/$OLDBUNDLE" "$DMGDIR"
macdeployqt "$DMGDIR/$OLDBUNDLE" -verbose=3
mv "$DMGDIR/$OLDBUNDLE" "$DMGDIR/$BUNDLE"

# Create a symlink to /Applications in the build directory
ln -s "/Applications" "$DMGDIR"

# Create disk image
# The window must be 46 pixels taller than the background image
chmod +x "$CREATEDMGDIR/create-dmg"
chmod +x "$CREATEDMGDIR/support/AdiumApplescriptRunner"
"$CREATEDMGDIR/create-dmg" \
    --volname "$FRIENDLYNAME" \
    --background "dmg-background.png" \
    --window-size 600 396 \
    --icon-size 128 \
    --icon "$BUNDLE" 160 100 \
    --icon "Applications" 440 100 \
    "$DISKIMAGE" \
    "$DMGDIR"

hdiutil internet-enable -yes "$DISKIMAGE"

# Remove temporary folder
rm -rf "$DMGDIR"
