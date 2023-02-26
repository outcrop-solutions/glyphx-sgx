@ECHO OFF

rem This script is for copying 3rdParty\bin and other files to the directory where everything is built

SET basedir=..\..\bin
SET antztemplate=GlyphPortableWin64
SET antzmactemplate=GlyphPortableOSX
SET defaultbaseimages=DefaultBaseImages
SET glyphtemplates=GlyphTemplates
SET meshes=meshes
SET shaders=shaders
SET fonts=fonts
SET textures=textures
SET qtdlllist=Qt6Core Qt6Gui Qt6Network Qt6OpenGL Qt6Sql Qt6Widgets Qt6WebEngineCore Qt6WebEngine Qt6WebEngineWidgets Qt6Quick Qt6WebChannel Qt6Qml Qt6WebSockets Qt6OpenGLWidgets Qt6QuickWidgets Qt6PrintSupport Qt6QmlModels Qt6Positioning

if not exist %basedir% (mkdir %basedir%)

robocopy /z /e ..\bin\ %basedir%

rem for each platform %%p
FOR /F "tokens=*" %%p IN ('dir /b /a:d ..\bin\*') DO (

	rem for each configuration %%c
	FOR /F "tokens=*" %%c IN ('dir /b /a:d ..\bin\%%p\*') DO (

		rem echo %%p 
		rem echo %%c 

		robocopy /z /e ..\tools\vc140redist\ %basedir%\%%p\%%c *.dll
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%defaultbaseimages% %basedir%\%%p\%%c\%defaultbaseimages%
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%glyphtemplates% %basedir%\%%p\%%c\%glyphtemplates%

		robocopy /z /e ..\..\Misc\InstallerFiles\%meshes% %basedir%\%%p\%%c\%meshes%
		robocopy /z /e ..\..\Misc\InstallerFiles\%shaders% %basedir%\%%p\%%c\%shaders%
		robocopy /z /e ..\..\Misc\InstallerFiles\%fonts% %basedir%\%%p\%%c\%fonts%
		robocopy /z /e ..\..\Misc\InstallerFiles\%textures% %basedir%\%%p\%%c\%textures%
		
		IF /I %%c==debug (
		
			FOR %%d IN (%qtdlllist%) DO (
			
				copy /B /Y "%QTDIR%\bin\%%dd.dll" "%basedir%\%%p\%%c\%%dd.dll"
			)
			copy /B /Y "%QTDIR%\bin\QtWebEngineProcessd.exe" "%basedir%\%%p\%%c\QtWebEngineProcessd.exe"
		) ELSE (
			
			FOR %%d IN (%qtdlllist%) DO (
			
				copy /B /Y "%QTDIR%\bin\%%d.dll" "%basedir%\%%p\%%c\%%d.dll"
			)
			copy /B /Y "%QTDIR%\bin\QtWebEngineProcess.exe" "%basedir%\%%p\%%c\QtWebEngineProcess.exe"
		)
		
		mkdir %basedir%\%%p\%%c\qt_plugins
		robocopy /z /e "%QTDIR%\plugins" %basedir%\%%p\%%c\qt_plugins
		
		robocopy /z /e "%QTDIR%\resources" "%basedir%\%%p\%%c"

		robocopy /z /e ..\tools\graphics\%%p\ %basedir%\%%p\%%c
	)
)
