@echo off

set projName=vania
pushd C:\Users\%username%\DeskTop\Projects\coding\%projName%

if %errorlevel% EQU 0 ( call build_win32.bat )

popd
