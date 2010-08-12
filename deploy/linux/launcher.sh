#!/bin/sh
appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname
export LD_LIBRARY_PATH
$dirname/$appname $*

# This will update the desktop entry file's icon path each time this script is run
mv silverlock.desktop silverlock.desktop-bak
sed -e "s,Icon=.*,Icon=$PWD/app.svg,g" silverlock.desktop-bak > silverlock.desktop
rm silverlock.desktop-bak
