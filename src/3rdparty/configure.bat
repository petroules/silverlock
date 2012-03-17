@echo off
echo This script configures 3rd party code for building with Petroules applications.
echo Copyright (c) 2010-2011 Petroules Corporation
echo.

REM Set physical directory of batch file
pushd %~dp0
set BUILDDIR=%CD%\temp-win32-msvc

REM Clean previous make file and temp directory
if exist "%CD%\make.bat" (
	del "%CD%\make.bat"
)

if exist "%BUILDDIR%" (
	rmdir /s /q "%BUILDDIR%"
)

if "%1"=="--clean" (
	echo Removed temporary files
	goto EOF
)

md "%BUILDDIR%"

REM Set default path to 7-Zip
set PATHTO7Z=%PROGRAMFILES%\7-Zip
if not exist "%PATHTO7Z%" (
	set /P PATHTO7Z=Could not find 7-Zip in default location. Please enter the location of 7z.exe: %=%
)

REM Add 7-Zip to path
set PATH=%PATH%;%PATHTO7Z%

REM Set filenames
set BOTAN_FN=Botan-1.10.1
set QTSLNS_FN=qt-solutions-qt-solutions
set QTSLNS_AFN=%QTSLNS_FN%-master
set SYNTEZA_FN=synteza-synteza
set SYNTEZA_AFN=%SYNTEZA_FN%-master

REM Set directory locations
set ARCHIVES_DIR=%CD%\archives
set PETROULES_DIR=%CD%\petroules
set OUT_DIR=%BUILDDIR%

echo Extracting Botan source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%BOTAN_FN%.tgz
7z x -o%OUT_DIR% %OUT_DIR%\%BOTAN_FN%.tar %BOTAN_FN%
move %OUT_DIR%\%BOTAN_FN% %OUT_DIR%\botan

echo Extracting Qt Solutions source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%QTSLNS_AFN%.tar.gz
7z x -o%OUT_DIR% %OUT_DIR%\%QTSLNS_AFN%.tar %QTSLNS_FN%
move %OUT_DIR%\%QTSLNS_FN% %OUT_DIR%\qtsolutions

echo Extracting Synteza source...
7z e -o%OUT_DIR% %ARCHIVES_DIR%\%SYNTEZA_AFN%.tar.gz
7z x -o%OUT_DIR% %OUT_DIR%\%SYNTEZA_AFN%.tar %SYNTEZA_FN%
move %OUT_DIR%\%SYNTEZA_FN% %OUT_DIR%\synteza

echo Copying modified QtSingleApplication files...
copy "%PETROULES_DIR%\qtsingleapplication.cpp" "%OUT_DIR%\qtsolutions\qtsingleapplication\src"
copy "%PETROULES_DIR%\qtsingleapplication.h" "%OUT_DIR%\qtsolutions\qtsingleapplication\src"
copy "%PETROULES_DIR%\botan.bat" "%OUT_DIR%\botan"
copy "%PETROULES_DIR%\android.txt" "%OUT_DIR%\botan\src\build-data\os"

echo Creating make script...
echo call "%BUILDDIR%\botan\botan.bat" > "%CD%\make.bat"

popd

:EOF
