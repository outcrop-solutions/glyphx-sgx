package synglyphx.util;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;

public class ErrorHandler {

	private static ErrorHandler instance = null;
	private ArrayList<StringWriter> errors = null;
	private boolean antz_csvs_written;
	private boolean sqlite_db_written;

	private ErrorHandler(){
		antz_csvs_written = false;
		sqlite_db_written = false;
		errors = new ArrayList<StringWriter>();
	}

	public static ErrorHandler getInstance(){
		if(instance == null){
			instance = new ErrorHandler();
		}
		return instance;
	}

	public PrintWriter addError() throws Exception{
		errors.add(new StringWriter());
		PrintWriter pw = new PrintWriter(errors.get(errors.size()-1));
		return pw;
	}

	public String[] getErrors(){
		String[] temp = new String[errors.size()];
		for(int i = 0; i < errors.size(); i++){
			temp[i] = errors.get(i).toString();
		}
		return temp;
	}

	public void clearErrors(){
		errors = new ArrayList<StringWriter>();
		antz_csvs_written = false;
		sqlite_db_written = false;
	}

	public void csvWriterCompleted(){
		antz_csvs_written = true;
	}

	public void sqliteWriterCompleted(){
		sqlite_db_written = true;
	}

	public int hasErrors(){
		if(errors.size() > 0){
			if(antz_csvs_written && sqlite_db_written){
				return 2;
			}
			return 1;
		}
		return 0;
	}
}
