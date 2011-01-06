@echo off

echo This script builds Botan using MSVC.
echo This file is in the public domain.

REM If we want to build in debug mode, then enable debug mode...
IF "%1"=="--debug" (
	set DEBUG_BUILD="--enable-debug"
)

REM Configure Botan using its Python configure script
REM Here we want to make sure to select x86 processor as building 64-bit Qt-based Petroules apps is not yet supported
configure.py --os=windows --cc=msvc --cpu=x86 --disable-shared --disable-asm

REM Build Botan and check it
nmake
nmake check
