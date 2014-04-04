@ECHO OFF

rem This script is for copying Qt files to the 3rdParty repository

robocopy /z %1\ %2\Release *.dll
robocopy /z %1\ %3\Release *.lib

robocopy /z /mov %2\Release\ %2\Debug *d.dll
robocopy /z /mov %3\Release\ %3\Debug *d.lib