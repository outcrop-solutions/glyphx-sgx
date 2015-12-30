@ECHO OFF

rem This script is for copying 3rdParty\bin and other files to the directory where everything is built

SET basedir=..\..\bin
SET antztemplate=ANTzTemplate
SET antzzspacetemplate=ANTzzSpaceTemplate
SET antzmactemplate=ANTzMacTemplate
SET defaultbaseimages=DefaultBaseImages
SET glyphtemplates=GlyphTemplates
SET logo=logo.png

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
	)
)