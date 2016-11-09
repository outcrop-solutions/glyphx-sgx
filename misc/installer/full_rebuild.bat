@echo off

echo Cleaning up previous installer build outputs...
call clean >NUL 2>NUL

echo Cleaning up output folder...
cd ../../bin/Win32
rd /s /q Release 2>NUL

echo Setting up output folder...
cd ../../3rdparty/scripts
call setupWindowsOutputBin >NUL 2>NUL
cd ../../misc/installer

echo Harvesting shared files...
heat dir ..\..\bin\Win32\Release\ -o harvest_shared.wxs -gg -scom -frag -srd -sreg -dr APPDIR -cg GlyphSharedComponentGroupID >NUL 2>NUL

echo Cleaning up project folder...
cd ../..
rd /s /q vs 2>NUL

echo Running cmake...
mkdir vs
cd vs
cmake ../gui -G "Visual Studio 12 2013" >NUL 2>NUL

echo Building applications (this will take a while)...
msbuild GlyphBuilder.sln /target:Rebuild /maxcpucount /p:Configuration=Release >NUL 2>NUL
cd ../misc/installer

call build_installers