@echo off
echo Make sure to remove built EXEs and any temporary files from your bin/Win64/Release directory
echo But also make sure all the SGX DLLs ARE in the release dir!
pause

echo Building shared_files.wxs...
heat dir ..\..\bin\Win64\Release\ -o shared_files.wxs -gg -scom -frag -srd -sreg -dr APPDIR -cg SharedFilesID >NUL
pause
