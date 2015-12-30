package synglyphx.util;

//import java.nio.file.*;

public class ConvertHash {
	
	public native String getHash(String id, String t, String f);

	public ConvertHash(){

	}

	static {
		//Path currentRelativePath = Paths.get("");
		//String s = currentRelativePath.toAbsolutePath().toString();
		//System.out.println("Current relative path is: " + s);
		System.loadLibrary("ConvertHash");
		
	}
}