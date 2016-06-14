@ECHO OFF

rem This script is for copying 3rdParty\bin and other files to the directory where everything is built

SET basedir=%1
SET antztemplate=ANTzTemplate
SET antzzspacetemplate=ANTzzSpaceTemplate
SET antzmactemplate=ANTzMacTemplate
SET defaultbaseimages=DefaultBaseImages
SET glyphtemplates=GlyphTemplates
SET logo=logo.png
SET qtdlllist=Qt5Core Qt5Gui Qt5Network Qt5OpenGL Qt5Sql Qt5Widgets

if "%basedir%." == "." SET basedir=..\..\bin

if not exist %basedir% (mkdir %basedir%)

robocopy /z /e ..\bin\ %basedir%

FOR /F "tokens=*" %%p IN ('dir /b /a:d ..\bin\*') DO (

	FOR /F "tokens=*" %%c IN ('dir /b /a:d ..\bin\%%p\*') DO (

		rem echo %%p 
		rem echo %%c 
		
		robocopy /z /e ..\..\ANTzPlus\%antztemplate% %basedir%\%%p\%%c\%antztemplate%
		robocopy /z /e ..\tools\vc120redist\ %basedir%\%%p\%%c\%antztemplate% *.dll
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\csv
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\images
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\plugin

		robocopy /z /e ..\..\ANTzPlus\%antzzspacetemplate% %basedir%\%%p\%%c\%antzzspacetemplate%
		robocopy /z /e ..\tools\vc120redist\ %basedir%\%%p\%%c\%antzzspacetemplate% *.dll
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\csv
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\images
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\plugin
		
		robocopy /z /e ..\..\ANTzPlus\%antzmactemplate% %basedir%\%%p\%%c\%antzmactemplate%
		mkdir %basedir%\%%p\%%c\%antzmactemplate%\usr\csv
		mkdir %basedir%\%%p\%%c\%antzmactemplate%\usr\images
		mkdir %basedir%\%%p\%%c\%antzmactemplate%\usr\plugin
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%defaultbaseimages% %basedir%\%%p\%%c\%defaultbaseimages%
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%glyphtemplates% %basedir%\%%p\%%c\%glyphtemplates%
		
		copy /B /Y ..\..\Misc\InstallerFiles\Images\%logo% %basedir%\%%p\%%c\%logo%
		
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
				copy /B /Y "%QTDIR%\bin\%%dd.dll" "%basedir%\%%p\%%c\%%dd.dll"	
			)
		) ELSE (
			
			FOR %%d IN (%qtdlllist%) DO (
			
				copy /B /Y "%QTDIR%\bin\%%d.dll" "%basedir%\%%p\%%c\%%d.dll"
				copy /B /Y "%QTDIR%\bin\%%d.dll" "%basedir%\%%p\%%c\%%d.dll"	
			)
		)
		
		mkdir %basedir%\%%p\%%c\qt_plugins
		robocopy /z /e "%QTDIR%\plugins" %basedir%\%%p\%%c\qt_plugins
	)
)