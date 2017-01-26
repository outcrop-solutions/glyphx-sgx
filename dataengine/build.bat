@ECHO OFF



SET jp=%cd%\..\JavaDataEngine

if not exist %cd%\DataEngine.vcxproj SET jp=%cd%\JavaDataEngine

SET jcmd=%jp%\..\jdk\bin

SET drivers=%jp%\database-drivers

SET CLASSPATH=%jp%;%drivers%\opencsv-3.7.jar;%drivers%\commons-lang3-3.1.jar;%drivers%\ojdbc7.jar;%drivers%\mysql-connector-java-5.1.38-bin.jar;%drivers%\sqlite-jdbc-3.8.11.2.jar;%drivers%\vertica-jdbc-7.2.1-0.jar;%drivers%\jsch-0.1.53.jar;

SET outpath=%jp%\..\..\classes

SET package=%outpath%\synglyphx



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



SET currdir=%cd%

cd %outpath%



%jcmd%\jar cf dataengine.jar DataEngine.class GlyphEngine.class UserAccessControls.class synglyphx\data\*.class synglyphx\glyph\*.class synglyphx\io\*.class synglyphx\util\*.class synglyphx\jdbc\*.class synglyphx\jdbc\driver\*.class synglyphx\link\*.class synglyphx\user\*.class



if not "%~1"=="" (

	if "%~1"=="run" (

		if not "%~2"=="" (

			if "%~2"=="-g" %jcmd%\java -Xmx4g -XX:+UseConcMarkSweepGC GlyphEngine

			if "%~2"=="-d" %jcmd%\java DataEngine

			if "%~2"=="-u" %jcmd%\java UserAccessControls

		)

	)

)

cd %currdir%