#!/bin/bash

# Script for preparing OSX app bundle (must be run after XCode build, from 3rdParty/scripts directory).
# Todo:
# - Some way to change hardcoded paths (command line option? store in a .conf file?)
# - Error handling (check for Qt, VM, etc and emit an error if not found)

build=None
app=None
java=7
appcount=0

while getopts "drvbmj:" opt; do
	case "$opt" in
		d)
			build=Debug
			;;
		r)
			build=Release
			;;
		v)
			app=GlyphViewer
			((appcount++))
			;;
		b)
			app=GlyphBuilder
			((appcount++))
			;;
		m)
			app=DataMapper
			((appcount++))
			;;
		j)
			if [ $OPTARG = 7 ]; then 
				java=7
			elif [ $OPTARG = 8 ]; then
				java=8
			else
				echo Invalid Java JVM version \'$OPTARG\'\; -j7 or j8 are supported.
				exit 1
			fi
			;;			
	esac
done

quit=false

if [ $build = None ]; then
	echo Build parameter required: specify either -d for Debug build or -r for Release build.
	quit=true
fi

if [ $appcount != 1 ]; then
	echo Exactly one app parameter required [got $appcount]:
	echo Specify -v for GlyphViewer, -b for GlyphBuilder, -m for DataMapper.
	quit=true
fi

if [ $quit = true ]; then
	exit 1
fi

echo Setting up $app bundle [$build] with JVM $java...

echo Deploying Qt...
/Users/Shared/Qt5.6.0/5.6/clang_64/bin/macdeployqt ../../cmake/bin/OSX64/$build/$app.app

if [ $java = 7 ]; then
	echo Deploying Java VM 1.7.0_79...
	cp -R /Library/Java/JavaVirtualMachines/jdk1.7.0_79.jdk ../../cmake/bin/OSX64/$build/$app.app/Contents/Frameworks
fi

if [ $java = 8 ]; then
	echo Deploying Java VM 1.8.0_92...
	cp -R /Library/Java/JavaVirtualMachines/jdk1.8.0_92.jdk ../../cmake/bin/OSX64/$build/$app.app/Contents/Frameworks
fi

echo Deploying Data Engine...
mkdir -p ../../cmake/bin/OSX64/$build/$app.app/Contents/Resources/DataEngine
cp -R ../../DataEngine/Java\ DataEngine/*.jar ../../cmake/bin/OSX64/$build/$app.app/Contents/Resources/DataEngine
cp -R ../../DataEngine/Java\ DataEngine/database-drivers ../../cmake/bin/OSX64/$build/$app.app/Contents/Resources/DataEngine
cp -R ../../DataEngine/Java\ DataEngine/converthash/libconverthash.dylib ../../cmake/bin/OSX64/$build/$app.app/Contents/MacOS

echo Deploying installation files...
cp -R ../../Misc/InstallerFiles/* ../../cmake/bin/OSX64/$build/$app.app/Contents/MacOS

# Todo: shouldn't be needed. Figure out why these files are being deployed here in the first place. (Are they
# build temporaries? Or maybe it's some misconfigured cmake step? Probably the latter...)
echo Cleaning up executable path...
rm -f ../../cmake/bin/OSX64/$build/*.dylib
rm -f ../../cmake/bin/OSX64/$build/*.jar

echo Done!
