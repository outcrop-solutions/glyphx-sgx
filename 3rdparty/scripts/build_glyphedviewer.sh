
# Make sure we're in the same path as this script since it expects that.
#cd "${0%/*}"

# Build GlyphEdViewer package.
echo Building GlyphEdViewer package...
#cd ../../../3rdparty/scripts
#./setup_osx.sh -ubir
cd ../../bin/OSX64/Release
cp ../../../misc/osx_resources/distribution_glyphedviewer.xml .
cp ../../../misc/InstallerFiles/Docs/EULA/license_agreement_glyphed.rtf LICENSE.rtf
cp ../../../misc/osx_resources/readme_ge.txt README.txt
productbuild --distribution distribution_glyphedviewer.xml --package-path . --resources . GlyphEdViewer.pkg
