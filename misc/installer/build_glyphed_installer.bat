@echo off
echo Building glyphed installer...
candle -arch x64 glyphed.wxs shared_files.wxs
light -ext WixUIExtension -cultures:en-us glyphed.wixobj shared_files.wixobj -o output/glyphed.msi -b ..\..\bin\Win64\Release\
