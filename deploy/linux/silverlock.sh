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
mv "$dirname/silverlock.desktop" "$dirname/silverlock.desktop-bak"
sed -e "s,Icon=.*,Icon=$dirname/app.svg,g" "$dirname/silverlock.desktop-bak" > "$dirname/silverlock.desktop"
rm "$dirname/silverlock.desktop-bak"
