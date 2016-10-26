import os

cd = os.getcwd()
jp = cd+'/../JavaDataEngine'

if not os.path.exists(cd+'/DataEngine.vcxproj'):
	jp = cd+'/JavaDataEngine'

jcmd = jp+'/../jdk/bin'
driver_path = jp+'/database-drivers/'

classpath = jp+';'
driver_set = {'opencsv-3.7.jar',
			'commons-lang3-3.1.jar',
			'ojdbc7.jar',
			'mysql-connector-java-5.1.38-bin.jar'
			'sqlite-jdbc-3.8.11.2.jar',
			'vertica-jdbc-7.2.1-0.jar',
			'jsch-0.1.53.jar'}

for driver in driver_set:
	classpath += driver_path+driver+';'

os.environ['CLASSPATH'] = classpath

outpath = jp+'/../../classes'
package = outpath+'/synglyphx'

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


jcompile = jcmd+'/javac -d '+outpath+' '+jp+'/'

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
	os.system(jcompile+javafile)

os.chdir(outpath)

jar = jcmd+'/jar cf dataengine.jar ' 
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
os.chdir(cd)