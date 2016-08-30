@ECHO OFF

rem This script is for copying Qt files to the 3rdParty repository
rem copyQt.bat SourceDir BinDir LibDir

robocopy /z %1\bin %2\Release *.dll
robocopy /z %1\bin %3\Release *.lib

robocopy /z /mov %2\Release\ %2\Debug *d.dll
robocopy /z /mov %3\Release\ %3\Debug *d.lib
robocopy /z %1\bin %2\Debug *.pdb

robocopy /z /e %1\plugins %2\Release\qt_plugins *.*
robocopy /z /e /mov %2\Release\qt_plugins %2\Debug\qt_plugins *d.dll
robocopy /z /e /mov %2\Release\qt_plugins %2\Debug\qt_plugins *.pdb