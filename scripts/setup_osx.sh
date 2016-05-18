#!/bin/bash

# Script for preparing OSX app bundle (must be run after XCode build, from 3rdParty/scripts directory).
# You will need to provide execute permission for this script before running it the first time:
#   chmod +x ./setup_osx.sh
# Todo:
# - Some way to change hardcoded paths (command line option? store in a .conf file?)
# - Error handling (check for Qt, VM, etc and emit an error if not found)

build=None
app=None
java=7
appcount=0

# Process command-line options.

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
			app=GlyphDesigner
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

# Validate command line options and quit with a message if invalid.

quit=false

if [ $build = None ]; then
	echo Build parameter required: specify either -d for Debug build or -r for Release build.
	quit=true
fi

if [ $appcount != 1 ]; then
	echo Exactly one app parameter required [got $appcount]:
	echo Specify -v for GlyphViewer, -b for GlyphDesigner, -m for DataMapper.
	quit=true
fi

if [ $quit = true ]; then
	exit 1
fi

echo Setting up $app bundle [$build] with JVM $java...

# This shouldn't be necessary but I haven't been able to figure out how to get cmake to set up xcode
# to drop the libraries in the bundle and link the app against them properly. So just moving them into
# it before running macdeployqt does the trick.
echo Moving libraries into app bundle...
mkdir -p ../../cmake/bin/OSX64/$build/$app.app/Contents/Frameworks
mv ../../cmake/bin/OSX64/$build/*.dylib ../../cmake/bin/OSX64/$build/$app.app/Contents/Frameworks 2>/dev/null

# NOTE: We're piping all the macdeployqt output to /dev/null. It spams a LOT of unnecessary text. Run it
# from the console if you need to see its output for some reason (or remove the two redirections starting
# with >).
echo Deploying Qt...
/Users/Shared/Qt5.6.0/5.6/clang_64/bin/macdeployqt ../../cmake/bin/OSX64/$build/$app.app >/dev/null 2>/dev/null

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
cp -R ../../DataEngine/Java\ DataEngine/libsqlite4java-osx.dylib ../../cmake/bin/OSX64/$build/$app.app/Contents/MacOS

echo Deploying installation files...
cp -R ../../Misc/InstallerFiles/* ../../cmake/bin/OSX64/$build/$app.app/Contents/MacOS

# Todo: shouldn't be needed. Figure out why this is deployed here in the first place...
echo Cleaning up executable path...
rm -f ../../cmake/bin/OSX64/$build/*.jar

echo Done!
