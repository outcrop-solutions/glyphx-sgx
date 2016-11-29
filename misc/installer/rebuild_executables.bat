@echo off

echo Cleaning up output folder...
cd ../../bin/Win32
rd /s /q Release 2>NUL

echo Setting up output folder...
cd ../../3rdparty/scripts
call setupWindowsOutputBin >NUL 2>NUL
cd ../../misc/installer

echo Cleaning up project folder...
cd ../..
rd /s /q vs 2>NUL

echo Running cmake...
mkdir vs
cd vs
cmake ../gui -G "Visual Studio 12 2013" >NUL

echo Building applications (this will take a while)...
msbuild GlyphBuilder.sln /target:Rebuild /maxcpucount /p:Configuration=Release >NUL
cd ../misc/installer
