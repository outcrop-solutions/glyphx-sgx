package synglyphx.io;

import java.util.Date;
import java.util.HashMap;
import java.util.ArrayList;
import com.almworks.sqlite4java.*;
import java.io.File;
import synglyphx.data.DataFrame;
import synglyphx.data.SourceDataInfo;

/*
TABLE INDEX
	TableName | FormattedName | Timestamp
	UUID        .csv			int

UUID TABLENAME
	FieldNames from data source
	FieldValues from data source
*/

public class SQLiteWriter {

	private ArrayList<SourceDataInfo> dataframes = null;
	private String outDir;

	public SQLiteWriter(ArrayList<SourceDataInfo> sdi, String outDir){
		this.dataframes = sdi;
		this.outDir = outDir;
		File file = new File(outDir+"\\sourcedata.db");
		file.delete();
	}

	public void writeTableIndex(){

		try{
			SQLiteConnection db = new SQLiteConnection(new File(outDir+"\\sourcedata.db"));
			db.open(true);
			String query = "CREATE TABLE if NOT EXISTS 'TableIndex' "; 
			query += "(TableName varchar(255), FormattedName varchar(255), Timestamp timestamp);";
			SQLiteStatement st0 = db.prepare(query);

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			SQLiteStatement st1 = db.prepare("INSERT INTO 'TableIndex' VALUES (?,?,?);", true);
			for(int i = 0; i < dataframes.size(); i++){
				SourceDataInfo sdi = dataframes.get(i);
				File file = new File(sdi.getPath());
				st1.bind(1, sdi.getFormattedID())
				.bind(2, sdi.getFormattedName())
				.bind(3, file.lastModified());
				st1.step(); 
				st1.reset();
			} 
			db.exec("COMMIT TRANSACTION;"); 
			db.dispose();
		}catch(SQLiteException se){
			se.printStackTrace();
		}
	}

	public void writeAllTables(){

		try{
			SQLiteConnection db = new SQLiteConnection(new File(outDir+"\\sourcedata.db"));
			db.open(true);

			for(int i = 0; i < dataframes.size(); i++){
				SourceDataInfo sdi = dataframes.get(i);
				writeTable(db,sdi);
			}

			//db.exec("COMMIT TRANSACTION;"); 
			db.dispose();
		}catch(SQLiteException se){
			se.printStackTrace();
		}
	}

	private void writeTable(SQLiteConnection db, SourceDataInfo sdi) throws SQLiteException{

		DataFrame data = sdi.getDataFrame();
		ArrayList<String> headers = data.getHeaders();
		HashMap<String, Boolean> fieldType = sdi.getDataFrame().getFieldType();

		String query = "CREATE TABLE if NOT EXISTS '"+sdi.getFormattedID()+"' ("; 
		String insertQuery = "INSERT INTO '"+sdi.getFormattedID()+"' VALUES (";
		for(int i = 0; i < headers.size(); i++){
			query += headers.get(i);
			if(fieldType.get(headers.get(i))){
				query += " REAL,";
			}else{
				query += " TEXT,";
			}
			insertQuery += "?,";
		}

		query = query.substring(0, query.length()-1);
		insertQuery = insertQuery.substring(0, insertQuery.length()-1);
		query += ");";
		insertQuery += ");";

		SQLiteStatement st0 = db.prepare(query);

		db.exec("BEGIN TRANSACTION;"); 
		st0.step(); 

		SQLiteStatement st1 = db.prepare(insertQuery, true);
		int size = data.size();
		for(int i = 0; i < size; i++){
			ArrayList<String> row = data.getRow(i);
			for(int j = 0; j < row.size(); j++){
				st1.bind(j+1, row.get(j));
			}
			st1.step(); 
			st1.reset();
		}

		db.exec("COMMIT TRANSACTION;"); 
	}

}