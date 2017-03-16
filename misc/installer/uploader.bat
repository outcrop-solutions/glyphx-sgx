@ECHO OFF

SET jcmd=%cd%\..\..\dataengine\JavaDataEngine\..\jdk\bin
SET drivers=%cd%\..\..\dataengine\JavaDataEngine\database-drivers
SET CLASSPATH=%cd%\*;%drivers%\*;

echo Starting to upload %~3

%jcmd%\java synglyphx/project/Uploader %~1 %~2 %~3

echo Finished uploading %~3

pause