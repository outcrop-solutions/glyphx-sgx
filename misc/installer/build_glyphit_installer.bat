@echo off
echo Building glyphit installer...
candle glyphit.wxs shared_files.wxs
light -ext WixUIExtension -cultures:en-us glyphit.wixobj shared_files.wixobj -o output/glyphit.msi -b ..\..\bin\Win32\Release\
