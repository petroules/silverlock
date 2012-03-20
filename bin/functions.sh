#!/bin/bash

# Colored echo function for the Bash shell
function cecho()
{
	if [[ "$2" == 'red' ]] ; then
		__color=";31"
	elif [[ "$2" == 'green' ]] ; then
		__color=";32"
	elif [[ "$2" == 'yellow' ]] ; then
		__color=";33"
	elif [[ "$2" == 'blue' ]] ; then
		__color=";34"
	elif [[ "$2" == 'magenta' ]] ; then
		__color=";35"
	elif [[ "$2" == 'cyan' ]] ; then
		__color=";36"
	elif [[ "$2" == 'white' ]] ; then
		__color=";37"
	else
		__color=";30" # black
	fi

	if [[ "$3" == 'bold' ]] ; then
		__bold="1"
	else
		__bold="0"
	fi

	echo -${4}e "\033[${__bold}${__color}m$1\033[0m"
}

function absolute-dirname()
{
	# http://stackoverflow.com/questions/1055671/how-can-i-get-the-behavior-of-gnus-readlink-f-on-a-mac
	local TARGET_FILE=$0

	cd `dirname $TARGET_FILE`
	TARGET_FILE=`basename $TARGET_FILE`

	# Iterate down a (possible) chain of symlinks
	while [ -L "$TARGET_FILE" ]
	do
		TARGET_FILE=`readlink $TARGET_FILE`
		cd `dirname $TARGET_FILE`
		TARGET_FILE=`basename $TARGET_FILE`
	done

	# Compute the canonicalized name by finding the physical path
	# for the directory we're in and appending the target file.
	local PHYS_DIR=`pwd -P`
	local RESULT=$PHYS_DIR/$TARGET_FILE
	local SCRIPT_DIR=`dirname $RESULT`
	echo $SCRIPT_DIR
	return 0
}
