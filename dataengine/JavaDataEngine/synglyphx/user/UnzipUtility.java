package synglyphx.user;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
 
public class UnzipUtility {

    private static final int BUFFER_SIZE = 4096;

    public static String unzip(String zipFilePath, String destDirectory) throws IOException {

        File destDir = new File(destDirectory);
        if (!destDir.exists()) {
            destDir.mkdir();
        }
        ZipInputStream zipIn = new ZipInputStream(new FileInputStream(zipFilePath));
        ZipEntry entry = zipIn.getNextEntry();

        // iterates over entries in the zip file
        String toReturn = entry.getName().split("/")[0];

        while (entry != null) {
            String filePath = destDirectory + File.separator + entry.getName();
            if (!entry.isDirectory()) {
                // if the entry is a file, extracts it
                extractFile(zipIn, filePath);
            } else {
                // if the entry is a directory, make the directory
                File dir = new File(filePath);
                new File(dir.getParent()).mkdirs();
                //dir.mkdir();
            }
            zipIn.closeEntry();
            entry = zipIn.getNextEntry();
        }
        zipIn.close();

        return toReturn;
    }

    private static void extractFile(ZipInputStream zipIn, String filePath) throws IOException {
        new File(filePath).getParentFile().mkdir();
        BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(new File(filePath)));
        byte[] bytesIn = new byte[BUFFER_SIZE];
        int read = 0;
        while ((read = zipIn.read(bytesIn)) != -1) {
            bos.write(bytesIn, 0, read);
        }
        bos.close();
    }

    public static void delete(String zipFilePath) throws IOException {
        File toDelete = new File(zipFilePath);
        deleteSubDir(toDelete);
        //toDelete.delete();
    }

    public static void deleteSubDir(File file) throws IOException{

        if(file.isDirectory()){
            if(file.list().length==0){
                file.delete();
            }else{
                //list all the directory contents
                String files[] = file.list();

                for (String temp : files) {
                    File fileDelete = new File(file, temp);
                    //recursive delete
                    deleteSubDir(fileDelete);
                }
                //check the directory again, if empty then delete it
                if(file.list().length==0){
                    file.delete();
                }
            }

        }else{
            //if file, then delete it
            file.delete();
        }
    }

}