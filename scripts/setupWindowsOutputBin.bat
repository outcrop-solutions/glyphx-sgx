@ECHO OFF

rem This script is for copying 3rdParty\bin to the directory where everything is built

SET basedir=..\..\bin
SET antztemplate=ANTzTemplate
SET antzzspacetemplate=ANTzzSpaceTemplate
SET defaultbaseimages=DefaultBaseImages
SET glyphtemplates=GlyphTemplates
SET logo=logo.png
SET exportlogo=logo_export.png

if not exist %basedir% (mkdir %basedir%)

robocopy /z /e ..\bin\ %basedir%

FOR /F "tokens=*" %%p IN ('dir /b /a:d ..\bin\*') DO (

	FOR /F "tokens=*" %%c IN ('dir /b /a:d ..\bin\%%p\*') DO (

		rem echo %%p 
		rem echo %%c 
		
		robocopy /z /e ..\..\ANTzPlus\%antztemplate% %basedir%\%%p\%%c\%antztemplate%
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\csv
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\images
		mkdir %basedir%\%%p\%%c\%antztemplate%\usr\plugin

		robocopy /z /e ..\..\ANTzPlus\%antzzspacetemplate% %basedir%\%%p\%%c\%antzzspacetemplate%
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\csv
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\images
		mkdir %basedir%\%%p\%%c\%antzzspacetemplate%\usr\plugin
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%defaultbaseimages% %basedir%\%%p\%%c\%defaultbaseimages%
		
		robocopy /z /e ..\..\Misc\InstallerFiles\%glyphtemplates% %basedir%\%%p\%%c\%glyphtemplates%
		
		copy /B /Y ..\..\Misc\InstallerFiles\Images\%logo% %basedir%\%%p\%%c\%logo%
		copy /B /Y ..\..\Misc\InstallerFiles\Images\%exportlogo% %basedir%\%%p\%%c\%exportlogo%
	)
)