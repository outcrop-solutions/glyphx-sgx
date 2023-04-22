import os

lib = 'QtPrintSupport'

install = 'install_name_tool -change '+lib+'.framework/Versions/5/'+lib 
install += ' @executable_path/..Frameworks/'+lib+'.framework/Versions/5/'+lib
install += ' ../PlugIns/printsupport/libcocoaprintersupport.dylib'

directory = '/Users/bryanholster/Documents/sgx/bin/OSX64/Debug/GlyphX.app/Contents'

def getFilesInDirectory(directory):
    for filename in os.listdir(directory):
        f = os.path.join(directory, filename)
        # checking if it is a file
        if os.path.isdir(f):
            getFilesInDirectory(f)
        elif f.endswith('.dylib'):
            #half = f.split('/PlugIns/')[1]
            path, fname = os.path.split(f)
            #print('/opt/homebrew/opt/qt@5/plugins/'+half)
            #install = 'install_name_tool -id @executable_path/../PlugIns/'+fname
            #install += ' '+f 
            install = 'otool '+f+' -L'
            os.system(install)

getFilesInDirectory(directory)
