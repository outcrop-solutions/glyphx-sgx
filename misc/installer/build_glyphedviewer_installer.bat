@echo off
echo Building glyphedviewer installer...
candle -arch x64 glyphedviewer.wxs shared_files.wxs
light -ext WixUIExtension -cultures:en-us glyphedviewer.wixobj shared_files.wixobj -o output/glyphedviewer.msi -b ..\..\bin\Win64\Release\
