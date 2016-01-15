package synglyphx.io;

import java.io.*;

public class Logger {

	private static Logger instance = null;

	private Logger(){}

	public static Logger getInstance(){
		if(instance == null){
			instance = new Logger();
			File file = new File("log.txt");
			file.delete();
		}

		return instance;
	}

	public static void add(String log){

		try{
			FileWriter f = new FileWriter("log.txt", true);  
			BufferedWriter bf = new BufferedWriter(f);

			bf.newLine();
			bf.write(log);
			bf.close();

	    }catch(IOException ioe){
	    	ioe.printStackTrace();
	    }
	}

	public static PrintStream addError() throws Exception{

		PrintStream ps = new PrintStream("log.txt");

		return ps;
	}

}