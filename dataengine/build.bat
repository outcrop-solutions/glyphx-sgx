@ECHO OFF



SET jp=%cd%\..\JavaDataEngine

if not exist %cd%\DataEngine.vcxproj SET jp=%cd%\JavaDataEngine

SET jcmd=%jp%\..\jdk\bin

SET drivers=%jp%\database-drivers

SET CLASSPATH=%jp%;%drivers%\*;

SET outpath=%jp%\..\..\classes

SET package=%outpath%\synglyphx


if not "%~3"=="x" (
if not exist %outpath% mkdir %outpath%

if not exist %package% mkdir %package%

if not exist %package%\data mkdir %package%\data

if not exist %package%\glyph mkdir %package%\glyph

if not exist %package%\io mkdir %package%\io

if not exist %package%\jdbc mkdir %package%\jdbc

if not exist %package%\jdbc\driver mkdir %package%\jdbc\driver

if not exist %package%\link mkdir %package%\link

if not exist %package%\util mkdir %package%\util

if not exist %package%\user mkdir %package%\user



%jcmd%\javac -d %outpath% %jp%\DataEngine.java

%jcmd%\javac -d %outpath% %jp%\GlyphEngine.java

%jcmd%\javac -d %outpath% %jp%\UserAccessControls.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\data\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\glyph\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\io\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\jdbc\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\jdbc\driver\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\link\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\util\*.java

%jcmd%\javac -d %outpath% %jp%\synglyphx\user\*.java

)

SET currdir=%cd%

cd %outpath%



%jcmd%\jar cf dataengine.jar DataEngine.class GlyphEngine.class UserAccessControls.class synglyphx\data\*.class synglyphx\glyph\*.class synglyphx\io\*.class synglyphx\util\*.class synglyphx\jdbc\*.class synglyphx\jdbc\driver\*.class synglyphx\link\*.class synglyphx\user\*.class



if not "%~1"=="" (

	if "%~1"=="run" (

		if not "%~2"=="" (

			if "%~2"=="-g" %jcmd%\java -Xmx4g -XX:+UseConcMarkSweepGC GlyphEngine

			if "%~2"=="-d" %jcmd%\java DataEngine

			if "%~2"=="-u" %jcmd%\java UserAccessControls

			if "%~2"=="-f" %jcmd%\java synglyphx.data.FieldProperties

		)

	)

)

cd %currdir%