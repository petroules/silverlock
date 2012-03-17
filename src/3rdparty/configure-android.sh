#!/bin/sh

#CC="--cc=gcc"
#CC_BIN="-cc-bin=g++|arm-linux-androideabi-g++"
#OS="--os=windows|macosx|linux|android|auto"
#CPU="--cpu=x86_64"
#DEBUG="--enable-debug"

TARGET_OPTS="$CC $CC_BIN $OS $CPU"
BUILD_OPTS="--disable-shared --disable-asm $DEBUG --no-autoload --gen-amalgamation"
MODS="--enable-modules=auto_rng,pbkdf2,dev_random,sha1"

./configure.py $TARGET_OPTS $BUILD_OPTS $MODS $@ --os=linux --cpu=arm

# Stop eclipse from whining about missing definitions because it is incompetent
# Also, -E is for BSD/OSX... GNU/Linux use -r
sed -E -i '' 's/std(::(memset|memcpy|memmove))/\1/g' botan_all.h

cp botan_all.* ~/Dropbox/Development/Eclipse/Silverlock/jni