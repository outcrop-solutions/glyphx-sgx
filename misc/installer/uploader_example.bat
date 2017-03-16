@ECHO OFF

rem Arg1, glyphed, is the bucket name in AWS
rem Arg2, testing/windows/random.txt, is the path of the file relative to it's bucket in AWS
rem Arg3, output/random.txt, is the relative local path to the file (An absolute path will work as well)

uploader glyphed testing/windows/random.txt output/random.txt

pause
