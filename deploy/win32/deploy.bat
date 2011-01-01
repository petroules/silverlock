@REM Build the installer
"C:\Program Files\NSIS\makensisw.exe" "silverlock.nsi"

@REM Move the compiled installer to the output directory
move "silverlock-1.1-windows.exe" "%CD%\..\..\..\silverlock-build-desktop\bin\silverlock-1.1-windows.exe"

pause
