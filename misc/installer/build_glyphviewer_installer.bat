@echo off
echo Building glyphviewer installer...
candle -arch x64 glyphviewer.wxs shared_files.wxs
light -ext WixUIExtension -cultures:en-us glyphviewer.wixobj shared_files.wixobj -o output/glyphviewer.msi -b ..\..\bin\Win64\Release\
echo Building glyphviewer DEMO installer...
light -dWixUILicenseRtf="../../misc/InstallerFiles/Docs/DEMO/license_agreement.rtf" -ext WixUIExtension -cultures:en-us glyphviewer.wixobj shared_files.wixobj -o output/glyphviewer_demo.msi -b ..\..\bin\Win64\Release\
