import os
import sys
import platform

print( "Building data engine..." )

cd = os.getcwd()
jp = cd+'/../JavaDataEngine'

if not os.path.exists(cd+'/DataEngine.vcxproj'):
	jp = cd+'/JavaDataEngine'

jcmd = jp+'/../jdk/bin/'
sep = ';'

if platform.system() == 'Darwin':
	jcmd = ''
	sep = ':'

driver_path = jp+'/database-drivers/'

classpath = jp+sep
classpath += driver_path+'*'+sep

outpath = jp+'/../../classes'
package = outpath+'/synglyphx'

if len(sys.argv) == 1:

	directories = {outpath,
				package,
				package+'/data',
				package+'/glyph',
				package+'/io',
				package+'/jdbc',
				package+'/jdbc/driver',
				package+'/link',
				package+'/util',
				package+'/user'}

	for directory in directories:
		if not os.path.exists(directory):
			os.makedirs(directory)


	jcompile = jcmd+'javac -d '+outpath+' '+jp+'/'

	javafiles = {'DataEngine.java',
				'GlyphEngine.java',
				'UserAccessControls.java',
				'synglyphx/data/*.java',
				'synglyphx/glyph/*.java',
				'synglyphx/io/*.java',
				'synglyphx/jdbc/*.java',
				'synglyphx/jdbc/driver/*.java',
				'synglyphx/link/*.java',
				'synglyphx/util/*.java',
				'synglyphx/user/*.java'}

	for javafile in javafiles:
		command = jcompile+javafile+" -classpath "+classpath
		os.system(command)

	os.chdir(outpath)

	jar = jcmd+'jar cf dataengine.jar ' 
	jar += 'DataEngine.class ' 
	jar += 'GlyphEngine.class ' 
	jar += 'UserAccessControls.class ' 
	jar += 'synglyphx/data/*.class ' 
	jar += 'synglyphx/glyph/*.class '
	jar += 'synglyphx/io/*.class ' 
	jar += 'synglyphx/util/*.class ' 
	jar += 'synglyphx/jdbc/*.class ' 
	jar += 'synglyphx/jdbc/driver/*.class ' 
	jar += 'synglyphx/link/*.class ' 
	jar += 'synglyphx/user/*.class'

	os.system(jar)

else:

	os.chdir(outpath)
	
	if sys.argv[1] == 'run':
		command = "java -classpath "+classpath+" GlyphEngine"
		print(command)
		os.system(command)

os.chdir(cd)
