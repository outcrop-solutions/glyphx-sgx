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
java=8
qt=5.6
appcount=0
do_build=0
do_install=0
clean_build=0
glyphed=0

# Process command-line options.

while getopts "drvegubcimj:q:" opt; do
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
		g)
			app=GlyphDesigner
			((appcount++))
			;;
		e)
			app=GlyphEd
			((appcount++))
			;;
		m)
			app=DataMapper
			((appcount++))
			;;
		u)
			app=GlyphEdViewer
			((appcount++))
			;;
        p)
            app=GlyphPortable
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
		q)
			if [ $OPTARG = 5.7 ]; then
				qt=5.7
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
	echo Specify -v for GlyphViewer, -g for GlyphDesigner, -m for DataMapper, -e for GlyphEd.
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

echo Setting up $app bundle [$build] with JVM $java...
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

if [ $java = 7 ]; then
	echo Deploying Java VM 1.7.0_79...
	cp -R /Library/Java/JavaVirtualMachines/jdk1.7.0_79.jdk ../../bin/OSX64/$build/$app.app/Contents/Frameworks
	echo Cleaning up unneeded Java files...
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/src.zip
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/man
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/db
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/bin
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/lib
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.7.0_79.jdk/Contents/Home/include
fi

if [ $java = 8 ]; then
	echo Deploying Java VM 1.8.0_131...
	# todo - don't hardcode JDK version number
	cp -R /Library/Java/JavaVirtualMachines/jdk1.8.0_131.jdk ../../bin/OSX64/$build/$app.app/Contents/Frameworks
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/src.zip
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/man
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/db
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/bin
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/lib
	rm -rf ../../bin/OSX64/$build/$app.app/Contents/Frameworks/jdk1.8.0_131.jdk/Contents/Home/include
fi

#echo Deploying Data Engine...
mkdir -p ../../bin/OSX64/$build/$app.app/Contents/Resources/DataEngine
cp ../../classes/dataengine.jar ../../bin/OSX64/$build/$app.app/Contents/Resources/DataEngine 2>/dev/null
cp -R ../../DataEngine/JavaDataEngine/database-drivers ../../bin/OSX64/$build/$app.app/Contents/Resources/DataEngine
cp -R ../../DataEngine/JavaDataEngine/converthash/libconverthash.dylib ../../bin/OSX64/$build/$app.app/Contents/MacOS

echo Deploying miscellaneous data files...
cp -R ../../Misc/InstallerFiles/* ../../bin/OSX64/$build/$app.app/Contents/MacOS

if [ $app = GlyphViewer ] || [ $app = GlyphEd ]; then
	echo Deploying ANTz Templates to $app app bundle...
#    cp -R ../../portable/GlyphPortableOSX ../../bin/OSX64/$build/$app.app/Contents/MacOS
#   cp -R ../../ANTzPlus/ANTzMacTemplate ../../bin/OSX64/$build/$app.app/Contents/MacOS
#	cp -R ../../ANTzPlus/ANTzTemplate ../../bin/OSX64/$build/$app.app/Contents/MacOS
#	cp -R ../../ANTzPlus/ANTzzSpaceTemplate ../../bin/OSX64/$build/$app.app/Contents/MacOS
#	cp ../tools/vc120redist/*.dll ../../bin/OSX64/$build/$app.app/Contents/MacOS/ANTzTemplate
#	cp ../tools/vc120redist/*.dll ../../bin/OSX64/$build/$app.app/Contents/MacOS/ANTzzSpaceTemplate
fi

echo Copying icon into app bundle...
if [ $app = GlyphEd ] || [ $app = GlyphEdViewer ]; then
	cp ../../Misc/osx_resources/glyphed.icns ../../bin/OSX64/$build/$app.app/Contents/Resources/synglyphx_x.icns
else
	cp ../../Misc/osx_resources/synglyphx_x.icns ../../bin/OSX64/$build/$app.app/Contents/Resources
fi

# Todo: shouldn't be needed. Figure out why this is deployed here in the first place...
echo Cleaning up executable path...
rm -f ../../bin/OSX64/$build/*.jar

if [ $do_install = 1 ]; then
#	echo Copying app bundle to desktop...
#	cd ../../bin/OSX64/$build
#	cp -R $app.app ~/Desktop
	echo Creating OSX package...
	cd ../../bin/OSX64/$build
	if [ $app = GlyphEdViewer ]; then
		install_path=/Applications/GlyphEdViewer.app
	fi
	if [ $app = GlyphEd ]; then
		install_path=/Applications/GlyphEd.app
	else
		install_path=/Applications/SynGlyphX/$app.app
	fi
	pkg_name=$app.component.pkg
	app_id=com.synglyphx.$app
	pkgbuild --root $app.app --identifier $app_id --install-location $install_path $pkg_name >/dev/null 2>/dev/null
fi

echo Done!
