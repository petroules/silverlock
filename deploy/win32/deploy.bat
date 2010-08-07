@REM Build the installer
"C:\Program Files\NSIS\makensisw.exe" "silverlock.nsi"

@REM Move the compiled installer to the output directory
move "silverlock-1.0-installer.exe" "%CD%\..\..\..\silverlock-build-desktop\bin\silverlock-1.0-installer.exe"

pause
