@ECHO OFF

rem This script is for copying 3rdParty\bin and other files to the directory where everything is built

SET basedir=%1
SET antztemplate=ANTzTemplate
SET antzzspacetemplate=ANTzzSpaceTemplate
SET antzmactemplate=ANTzMacTemplate
SET defaultbaseimages=DefaultBaseImages
SET glyphtemplates=GlyphTemplates
SET logo=logo.png

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
		
		robocopy /z /e ..\..\DataEngine\jdk1.7.0_79\jre\bin %basedir%\%%p\%%c\jre\bin *.dll
		robocopy /z /e ..\..\DataEngine\jdk1.7.0_79\jre\lib %basedir%\%%p\%%c\jre\lib
		
		mkdir %basedir%\%%p\%%c\database-drivers
		robocopy /z /e "..\..\DataEngine\Java DataEngine\database-drivers" %basedir%\%%p\%%c\database-drivers
		
		copy /B /Y "..\..\DataEngine\Java DataEngine\ojdbc6.jar" %basedir%\%%p\%%c\ojdbc6.jar
		copy /B /Y "..\..\DataEngine\Java DataEngine\ConvertHash.dll" %basedir%\%%p\%%c\ConvertHash.dll

		rem TODO: Needed files only.
		robocopy /z /e "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\*.dll" %basedir%\%%p\%%c
		robocopy /z /e "C:\Qt\Qt5.6.0\5.6\msvc2013\bin\*.dll" %basedir%\%%p\%%c
		mkdir %basedir%\%%p\%%c\qt_plugins
		robocopy /z /e "C:\Qt\Qt5.6.0\5.6\msvc2013\plugins" %basedir%\%%p\%%c\qt_plugins
	)
)