VER = 1203

JAR = rlmez$(VER).jar

SO = librlmez$(VER).so

all:	EZClient.class so

so:	../$(SO)
	cp ../$(SO) .

EZClient.class:	EZClient.java
	javac -classpath $(JAR) EZClient.java

runclient:	EZClient.class
	java -classpath "$(JAR):." EZClient

clean:
	rm -f $(SO) EZClient.class
