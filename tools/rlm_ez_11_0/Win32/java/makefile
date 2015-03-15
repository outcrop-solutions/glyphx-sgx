VER = 1102

JAR = rlmez$(VER).jar

DLL = rlmez$(VER).dll

all:	EZClient.class dll

dll:	..\$(DLL)
	copy ..\$(DLL) .

EZClient.class:	EZClient.java
	javac -classpath $(JAR) EZClient.java

runclient:	EZClient.class
	java -classpath "$(JAR);." EZClient

clean:
	del $(DLL) EZClient.class
