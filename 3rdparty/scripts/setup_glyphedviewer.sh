#!/bin/bash

app=GlyphEdViewer
echo Creating OSX package...
cd ../../bin/OSX64/Release
install_path=/Applications/GlyphEdViewer.app
pkg_name=$app.component.pkg
app_id=com.synglyphx.$app
pkgbuild --root $app.app --identifier $app_id --install-location $install_path $pkg_name >/dev/null 2>/dev/null

echo Done!
