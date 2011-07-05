@echo off
echo This script configures 3rd party code for building with Petroules applications.
echo Copyright (c) 2010-2011 Petroules Corporation

set BUILDDIR=temp-win32-msvc

rmdir /s /q "%BUILDDIR%"

IF "%1"=="--clean" (
	goto EOF
)

md "%BUILDDIR%"

REM Set default path to 7-Zip
set PATHTO7Z=%PROGRAMFILES%\7-Zip
IF NOT EXIST "%PATHTO7Z%" (
	set /P PATHTO7Z=Could not find 7-Zip in default location. Please enter the location of 7z.exe: %=%
)

REM Add 7-Zip to path
set PATH=%PATH%;%PATHTO7Z%

REM Set filenames
set BOTAN_FN=Botan-1.9.12
set QTSLNS_FN=qt-solutions-qt-solutions
set QTSLNS_AFN=%QTSLNS_FN%-master
set LIEL_FN=liel-liel
set LIEL_AFN=%LIEL_FN%-master

REM Set directory locations
set ARCHIVES_DIR=%CD%\archives
set PETROULES_DIR=%CD%\petroules
set OUT_DIR=%CD%\%BUILDDIR%

echo Extracting Botan source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%BOTAN_FN%.tgz
7z x -o%OUT_DIR% %OUT_DIR%\%BOTAN_FN%.tar %BOTAN_FN%
move %OUT_DIR%\%BOTAN_FN% %OUT_DIR%\botan
7z x -o%OUT_DIR% %OUT_DIR%\%BOTAN_FN%.tar %BOTAN_FN%
move %OUT_DIR%\%BOTAN_FN% %OUT_DIR%\botan-msvc

echo Extracting Qt Solutions source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%QTSLNS_AFN%.tar.gz
7z x -o%OUT_DIR% %OUT_DIR%\%QTSLNS_AFN%.tar %QTSLNS_FN%
move %OUT_DIR%\%QTSLNS_FN% %OUT_DIR%\qtsolutions

echo Extracting LIEL source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%LIEL_AFN%.tar.gz
7z x -o%OUT_DIR% %OUT_DIR%\%LIEL_AFN%.tar %LIEL_FN%
move %OUT_DIR%\%LIEL_FN% %OUT_DIR%\liel

echo Copying modified QtSingleApplication files...
copy "%PETROULES_DIR%\qtsingleapplication.cpp" "%OUT_DIR%\qtsolutions\qtsingleapplication\src"
copy "%PETROULES_DIR%\qtsingleapplication.h" "%OUT_DIR%\qtsolutions\qtsingleapplication\src"
copy "%PETROULES_DIR%\botan.sh" "%OUT_DIR%\botan"
copy "%PETROULES_DIR%\botan.bat" "%OUT_DIR%\botan-msvc"

:EOF
