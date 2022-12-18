#!/bin/bash

# Script for preparing OSX app bundle (must be run after XCode build, from 3rdParty/scripts directory).
# You will need to provide execute permission for this script before running it the first time:
#   chmod +x ./setup_osx.sh
# Todo:
# - Some way to change hardcoded paths (command line option? store in a .conf file?)
# - Error handling (check for Qt, VM, etc and emit an error if not found)

# Make sure we're in the same path as this script since it expects that.
# (Only do this if it isn't already set since we might be calling this from another script that set it already.)
if [ -z ${script_path+x} ]; then
	cd "${0%/*}"
	script_path="$(pwd)"
fi

cd $script_path

build=None
app=None
qt=5.14
appcount=0
do_build=0
do_install=0
clean_build=0
glyphed=0

# Process command-line options.

while getopts "drxbic" opt; do
	case "$opt" in
		d)
			build=Debug
			;;
		r)
			build=Release
			;;
		x)
			app=GlyphX
			((appcount++))
			;;
		b)
			do_build=1
			;;
		i)
			do_install=1
			;;
		c)
			clean_build=1
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
	echo Specify -x for GlyphX.
	quit=true
fi

if [ $quit = true ]; then
	exit 1
fi

if [ $do_build = 1 ]; then
	echo Building and deploying $app in $build configuration...
fi

# Build if asked
if [ $do_build = 1 ]; then
	cd ../..
	if [ $clean_build = 1 ]; then
		echo Cleaning up any previous build files...
		rm -rf cmake
		rm -rf xcode
	fi
	echo Running cmake...
	mkdir xcode 2>/dev/null
	cd xcode
	rm CMakeCache.txt 2>/dev/null
	cmake ../gui -G Xcode >/dev/null

	echo Building...
	xcodebuild -target $app -configuration $build -IDEBuildOperationMaxNumberOfConcurrentCompileTasks=4 >/dev/null
fi

# Return to script path since the rest of the script expects us to be there,
cd $script_path

echo Setting up $app bundle [$build]...
mkdir -p ../../bin/OSX64/$build/$app.app/Contents/Frameworks

echo Copying 3rd-party libraries into app bundle... 
cp ../tools/graphics/OSX64/*.dylib ../../bin/OSX64/$build/$app.app/Contents/Frameworks 2>/dev/null

# This shouldn't be necessary but I haven't been able to figure out how to get cmake to set up xcode
# to drop the libraries in the bundle and link the app against them properly. So just moving them into
# it before running macdeployqt does the trick.
echo Copying sgx libraries into app bundle...
cp ../../bin/OSX64/$build/*.dylib ../../bin/OSX64/$build/$app.app/Contents/Frameworks 2>/dev/null

# NOTE: We're piping all the macdeployqt output to /dev/null. It spams a LOT of unnecessary text. Run it
# from the console if you need to see its output for some reason (or remove the two redirections starting
# with >).
echo Deploying Qt...
/Users/Shared/Qt$qt.0/$qt/clang_64/bin/macdeployqt ../../bin/OSX64/$build/$app.app >/dev/null 2>/dev/null

echo Deploying miscellaneous data files...
cp -R ../../Misc/InstallerFiles/* ../../bin/OSX64/$build/$app.app/Contents/MacOS

echo Copying icon into app bundle...
cp ../../Misc/osx_resources/synglyphx_x.icns ../../bin/OSX64/$build/$app.app/Contents/Resources


# Todo: shouldn't be needed. Figure out why this is deployed here in the first place...
if [ $do_install = 1 ]; then
#	echo Copying app bundle to desktop...
#	cd ../../bin/OSX64/$build
#	cp -R $app.app ~/Desktop
	echo Creating OSX package...
	cd ../../bin/OSX64/$build
    install_path=/Applications/GlyphX.app

	pkg_name=$app.component.pkg
	app_id=com.synglyphx.$app
	pkgbuild --root $app.app --identifier $app_id --install-location $install_path $pkg_name >/dev/null 2>/dev/null
fi

echo Done!
