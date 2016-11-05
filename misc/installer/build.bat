heat dir ..\..\bin\Win32\Release\ -o harvest.wxs -gg -scom -frag -srd -sreg -dr APPDIR -cg GlyphITComponentGroupID
candle glyphit.wxs harvest.wxs
light -ext WixUIExtension -cultures:en-us glyphit.wixobj harvest.wixobj -o output/glyphit.msi -b ..\..\bin\Win32\Release\
rm -f output/*.wixpdb
