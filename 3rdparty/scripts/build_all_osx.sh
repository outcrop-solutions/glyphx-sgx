
# Script to do a one-step build of releases for all four apps.

# Make sure we're in the same path as this script since it expects that.
cd "${0%/*}"

echo Building all apps...

opts=birj8
opts_first=c$opts

no_clean=0
while getopts "n" opt; do
	case "$opt" in
		n)
			echo Skipping full clean...
			opts_first=$opts
			;;
	esac
done

# Build the three apps. Note we're only passing -c (clean build directories) for the first one,
# since doing it every time would be redundant and would make the build take forever.
./setup_osx.sh -g$opts_first
./setup_osx.sh -m$opts
./setup_osx.sh -v$opts

# Build an app package from the three packages we built.
echo Building app package...
cd ../../cmake/bin/OSX64/Release
cp ../../../../misc/osx_resources/distribution.xml .
cp ../../../../misc/osx_resources/readme.txt README.txt
cp ../../../../misc/InstallerFiles/Docs/EULA/license_agreement.rtf LICENSE.rtf
productbuild --distribution distribution.xml --package-path . --resources . GlyphBuilder.pkg
echo Building demo package...
cp ../../../../misc/InstallerFiles/Docs/Demo/license_agreement.rtf LICENSE.rtf
productbuild --distribution distribution.xml --package-path . --resources . GlyphBuilderDemo.pkg

# Build GlyphEd package.
echo Building GlyphEd package...
cd ../../../../3rdparty/scripts
./setup_osx.sh -ebirj8
cd ../../cmake/bin/OSX64/Release
cp ../../../../misc/osx_resources/distribution_glyphed.xml .
productbuild --distribution distribution_glyphed.xml --package-path . --resources . GlyphEd.pkg
