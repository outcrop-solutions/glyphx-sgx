@echo off
echo Building glyphx installer...
candle -arch x64 glyphx.wxs shared_files.wxs
light -ext WixUIExtension -cultures:en-us glyphx.wixobj shared_files.wixobj -o output/glyphx.msi -b ..\..\bin\Win64\Release\
