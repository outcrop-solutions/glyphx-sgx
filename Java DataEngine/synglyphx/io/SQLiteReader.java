package synglyphx.io;

import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;
import com.almworks.sqlite4java.*;
import synglyphx.data.DataFrame;

public class SQLiteReader {

	private DataFrame data = null;
	private ArrayList<String> tableNames = null;
	private int headers = 0;

	public SQLiteReader(){
		//System.loadLibrary("../../DataEngine/sqlite4java-392/sqlite4java-win32-x64");
	}

	public void createDataFrame(String path, String name){
		this.data = new DataFrame();
		this.readSQLite(path, name);
		data.createMinMaxTable();
	}

	public void readTables(String path){

		Logger.getInstance().add("Reading table names...");
		tableNames = new ArrayList<String>();
		SQLiteStatement st0 = null;
		try{
			SQLiteConnection db = new SQLiteConnection(new File(path));
			db.open(true);

		    st0 = db.prepare("SELECT name FROM sqlite_master WHERE type='table';"); 

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			int size = st0.columnCount();
			while(st0.hasRow()){
				String str = "";
				str += st0.columnValue(0);
				if(!str.equals("sqlite_sequence")){
					tableNames.add(str);
					headers++;
				}
				st0.step();
			}

			db.exec("COMMIT TRANSACTION;"); 

		}catch(SQLiteException se){
			se.printStackTrace();
			Logger.getInstance().add(se.getMessage());
		}
	}

	private void readSQLite(String path, String name){

		ArrayList<String> temp = null;
		SQLiteStatement st0 = null;

		Logger.getInstance().add("Reading table data...");

		try{
			SQLiteConnection db = new SQLiteConnection(new File(path));
			db.open(true);

		    st0 = db.prepare("SELECT * FROM "+name+";"); 

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			temp = new ArrayList<String>();
			int size = st0.columnCount();
			for(int i = 0; i<size; i++){
				String str = "";
				str += st0.getColumnName(i);
				temp.add(str);
			}
			this.data.addRow(temp);

			while(st0.hasRow()){
				temp = new ArrayList<String>();
				for(int i = 0; i<size; i++){
					String str = "";
					str += st0.columnValue(i);
					temp.add(str);
				}
				this.data.addRow(temp);
				st0.step();
			}

			db.exec("COMMIT TRANSACTION;"); 

		}catch(SQLiteException se){
			se.printStackTrace();
			Logger.getInstance().add(se.getMessage());
		}
	}

	public ArrayList<String> getTableNames(){
		return tableNames;
	}

	public DataFrame getDataFrame(){
		return data;
	}
/*
	public static void main(String [] args){
		SQLiteReader reader = new SQLiteReader();
		reader.createDataFrame("temp.sqlite");
	}*/
}