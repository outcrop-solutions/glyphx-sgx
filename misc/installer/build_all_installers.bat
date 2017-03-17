@echo off
echo Make sure you've set up your release directory and build all four executables before proceeding.
echo Also make sure you've updated the version numbers in version.wxi and in version.h.
pause
call build_glyphed_installer.bat
call build_glyphviewer_installer.bat
call build_glyphit_installer.bat
echo Done!

pause