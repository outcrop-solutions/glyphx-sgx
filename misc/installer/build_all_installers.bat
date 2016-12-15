@echo off
echo Make sure you've set up your release directory and build all four executables before proceeding.
echo Also make sure you've updated the version number in version.wxi.
pause
call build_glyphed_installer.bat
call build_glyphviewer_installer.bat
call build_glyphed_installer.bat
echo Done!
