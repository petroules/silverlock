#!/bin/sh
#
# Petroules Enterprises Qt Deployment script for Mac OS X
# Copyright © 2010 Petroules Enterprises

### get required user input #########################################

if [ -z $APPNAME ] ; then
    echo
    echo "This script prepares a Qt application bundle for deployment. It will"
    echo "copy over the required Qt frameworks and sets the installation"
    echo "identifications. Please see the \"Deploying an Application on Qt/Mac\""
    echo "page in the Qt documentation for more information."
    echo
    echo "This script assumes you have already built the application bundle."
    echo
    echo -n "What is the UNIX name of the application (this should be the name of the bundle)? "
    read userinput
    APPNAME=$userinput
	echo -n "What is the friendly/display name of the application? "
	read userinput
	FRIENDLYNAME=$userinput
fi

BUNDLE="$APPNAME.app"
APPBIN="$BUNDLE/Contents/MacOS/$APPNAME"

if [ ! -d "../$BUNDLE" ] ; then
    echo "ERROR: cannot find application bundle \"$BUNDLE\" in current directory"
    exit
fi

if [ ! -x "../$APPBIN" ] ; then
    echo "ERROR: cannot find application in bundle. Did you forget to run make?"
    exit
fi

echo "application:   $APPNAME"
echo "bundle:        $BUNDLE"
echo "friendly name: $FRIENDLYNAME"

mkdir mac
cp -R "../$BUNDLE" "./mac/$BUNDLE"
"/usr/bin/macdeployqt-4.6" "mac/$BUNDLE"

### create disk image ###############################################

echo "Creating disk image"
imagedir="/tmp/$APPNAME.$$"
mkdir "$imagedir"
cp -R "mac/$BUNDLE" "$imagedir"
# TODO: copy over additional files, if any

# Rename file to friendly name
mv "$imagedir/$BUNDLE" "$imagedir/$FRIENDLYNAME.app"

hdiutil create -ov -srcfolder "$imagedir" -format UDBZ -volname "$FRIENDLYNAME" "mac/$APPNAME.dmg"
hdiutil internet-enable -yes "mac/$APPNAME.dmg"
rm -rf "$imagedir"

echo "Done"
