package synglyphx.util;

import java.nio.file.*;
import java.io.*;

public class ConvertHash {
	
	public native String getHash(String id, String t, String f);

	public ConvertHash(){

	}

	static {/*
		Path currentRelativePath = Paths.get("");
		String s = currentRelativePath.toAbsolutePath().toString();
		//System.out.println("Current relative path is: " + s);
		try{
			FileWriter f = new FileWriter("path.txt", true);  
			BufferedWriter bf = new BufferedWriter(f);

			bf.write(s);
			bf.close();

	    }catch(IOException ioe){
	    	ioe.printStackTrace();
	    }*/
		System.loadLibrary("ConvertHash");
		//System.load("/home/admin/SynGlyphX/cmake/bin/Linux64/libConvertHash.so");
	}
}