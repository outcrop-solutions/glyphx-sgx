@echo off
echo Make sure to remove built EXEs and any temporary files from your bin/Win32/Release directory.
pause

echo Building shared_files.wxs...
heat dir ..\..\bin\Win32\Release\ -o shared_files.wxs -gg -scom -frag -srd -sreg -dr APPDIR -cg SharedFilesID >NUL
