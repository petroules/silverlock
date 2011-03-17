#!/bin/sh

echo
echo "This script creates a DMG file containing the application bundle, and a PKG file for the Mac App Store. It assumes you have already built the application bundle."
echo

APPNAME="silverlock"
FRIENDLYNAME="Silverlock"

OLDBUNDLE="$APPNAME.app"
BUNDLE="$FRIENDLYNAME.app"
DISKIMAGE="$APPNAME.dmg"
PACKAGE="$APPNAME.pkg"
SRCDIR="$PWD/../../$APPNAME-build-desktop/desktop"
TEMPDIR="$PWD/tmp"
CREATEDMGDIR="$PWD/../../src/3rdparty/temp/create-dmg"

if [ ! -d "$SRCDIR/$OLDBUNDLE" ] ; then
    echo "ERROR: cannot find application bundle \"$OLDBUNDLE\" in directory \"$SRCDIR\""
    exit
fi

# Remove previous disk image
if [ -f "$DISKIMAGE" ] ; then
    rm "$DISKIMAGE"
fi

# Remove previous application bundle
if [ -f "$PACKAGE" ] ; then
    rm "$PACKAGE"
fi

# Copy the bundle from the build directory, macdeployqt it, and rename it
mkdir "$TEMPDIR"
cp -R "$SRCDIR/$OLDBUNDLE" "$TEMPDIR"
macdeployqt "$TEMPDIR/$OLDBUNDLE" -verbose=3
mv "$TEMPDIR/$OLDBUNDLE" "$TEMPDIR/$BUNDLE"

# Create a symlink to /Applications in the build directory
ln -s "/Applications" "$TEMPDIR"

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
    "$TEMPDIR"

# Enable the disk image for automatic mounting, and installation of the application bundle
hdiutil internet-enable -yes "$DISKIMAGE"

# Enter our temporary directory to prepare to code-signing
cd "$TEMPDIR"

# Sign the application bundle
codesign -s "3rd Party Mac Developer Application: Petroules Corporation" -v "$BUNDLE"
codesign -v "$BUNDLE"

# Build the PKG file
productbuild --component "$BUNDLE" /Applications --sign "3rd Party Mac Developer Installer: Petroules Corporation" "$PACKAGE"

# Test the PKG file
sudo installer -store -pkg "$PACKAGE" -target /

# Move the package up a directory and go back
cd ..
mv "$TEMPDIR/$PACKAGE" "$PACKAGE"

# Remove temporary folder
sudo rm -rf "$TEMPDIR"
