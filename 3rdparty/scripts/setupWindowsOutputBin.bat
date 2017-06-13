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
SET qtdlllist=Qt5Core Qt5Gui Qt5Network Qt5OpenGL Qt5Sql Qt5Widgets Qt5WebEngineCore Qt5WebEngine Qt5WebEngineWidgets Qt5Quick Qt5WebChannel Qt5Qml

if not exist %basedir% (mkdir %basedir%)

robocopy /z /e ..\bin\ %basedir%

rem for each platform %%p
FOR /F "tokens=*" %%p IN ('dir /b /a:d ..\bin\*') DO (

	rem for each configuration %%c
	FOR /F "tokens=*" %%c IN ('dir /b /a:d ..\bin\%%p\*') DO (

		rem echo %%p 
		rem echo %%c 

		robocopy /z /e ..\tools\vc120redist_64\ %basedir%\%%p\%%c *.dll
		
		robocopy /z /e ..\..\portable\%antztemplate% %basedir%\%%p\%%c\%antztemplate%
		robocopy /z /e ..\..\portable\%antzmactemplate% %basedir%\%%p\%%c\%antzmactemplate%
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%defaultbaseimages% %basedir%\%%p\%%c\%defaultbaseimages%
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%glyphtemplates% %basedir%\%%p\%%c\%glyphtemplates%

		robocopy /z /e ..\..\Misc\InstallerFiles\%meshes% %basedir%\%%p\%%c\%meshes%
		robocopy /z /e ..\..\Misc\InstallerFiles\%shaders% %basedir%\%%p\%%c\%shaders%
		robocopy /z /e ..\..\Misc\InstallerFiles\%fonts% %basedir%\%%p\%%c\%fonts%
		robocopy /z /e ..\..\Misc\InstallerFiles\%textures% %basedir%\%%p\%%c\%textures%

		mkdir %basedir%\%%p\%%c\jre
		mkdir %basedir%\%%p\%%c\jre\bin
		mkdir %basedir%\%%p\%%c\jre\lib
		
		robocopy /z /e ..\..\DataEngine\jdk\jre\bin %basedir%\%%p\%%c\jre\bin *.dll
		robocopy /z /e ..\..\DataEngine\jdk\jre\lib %basedir%\%%p\%%c\jre\lib
		
		mkdir %basedir%\%%p\%%c\database-drivers
		robocopy /z /e "..\..\DataEngine\JavaDataEngine\database-drivers" %basedir%\%%p\%%c\database-drivers
		
		copy /B /Y "..\..\DataEngine\JavaDataEngine\ConvertHash.dll" %basedir%\%%p\%%c\ConvertHash.dll
		
		IF /I %%c==debug (
		
			FOR %%d IN (%qtdlllist%) DO (
			
				copy /B /Y "%QTDIR%\bin\%%dd.dll" "%basedir%\%%p\%%c\%%dd.dll"
			)
		) ELSE (
			
			FOR %%d IN (%qtdlllist%) DO (
			
				copy /B /Y "%QTDIR%\bin\%%d.dll" "%basedir%\%%p\%%c\%%d.dll"
			)
		)
		
		mkdir %basedir%\%%p\%%c\qt_plugins
		robocopy /z /e "%QTDIR%\plugins" %basedir%\%%p\%%c\qt_plugins
		
		copy /B /Y "%QTDIR%\bin\QtWebEngineProcess.exe" "%basedir%\%%p\%%c\QtWebEngineProcess.exe"

		robocopy /z /e "%QTDIR%\resources" "%basedir%\%%p\%%c"

		robocopy /z /e ..\tools\graphics\%%p\ %basedir%\%%p\%%c
	)
)
