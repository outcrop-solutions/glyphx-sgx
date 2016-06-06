package synglyphx.io;

import java.sql.*;
import java.util.Date;
import java.util.HashMap;
import java.util.ArrayList;
import com.almworks.sqlite4java.*;
import java.io.File;
import synglyphx.jdbc.Table;
import synglyphx.jdbc.BasicTable;
import synglyphx.jdbc.MergedTable;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;
import synglyphx.data.DataFrame;
import synglyphx.data.SourceDataInfo;
import synglyphx.glyph.XMLGlyphTemplate;

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
	private HashMap<Integer, XMLGlyphTemplate> templates;
	private ArrayList<Integer> rootIds;
	private ArrayList<Integer> toPrint;
	private String outDir;

	public SQLiteWriter(ArrayList<SourceDataInfo> sdi, String outDir, ArrayList<Integer> rootIds, HashMap<Integer, XMLGlyphTemplate> templates){
		this.dataframes = sdi;
		this.outDir = outDir;
		this.rootIds = rootIds;
		this.templates = templates;
		setToPrint();
		File file = new File(outDir+"/sourcedata.db");
		file.delete();
	}


	public void writeSDTInfo(String timestamp){

		try{
			SQLiteConnection db = new SQLiteConnection(new File(outDir+"/sourcedata.db"));
			db.open(true);
			String query = "CREATE TABLE if NOT EXISTS 'SDTInfo' (lastChanged varchar(255));";
			SQLiteStatement st0 = db.prepare(query);

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			SQLiteStatement st1 = db.prepare("INSERT INTO 'SDTInfo' VALUES (?);", true);

			st1.bind(1, timestamp);
			st1.step(); 
			st1.reset();
		
			db.exec("COMMIT TRANSACTION;"); 
			db.dispose();
		}catch(SQLiteException se){
			try{
	            se.printStackTrace(Logger.getInstance().addTError());
	        }catch(Exception e){}
		}
	}

	public void writeTableIndex(){

		try{
			SQLiteConnection db = new SQLiteConnection(new File(outDir+"/sourcedata.db"));
			db.open(true);
			String query = "CREATE TABLE if NOT EXISTS 'TableIndex' "; 
			query += "(TableName varchar(255), FormattedName varchar(255), Timestamp timestamp);";
			SQLiteStatement st0 = db.prepare(query);

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			SQLiteStatement st1 = db.prepare("INSERT INTO 'TableIndex' VALUES (?,?,?);", true);
			for(int i = 0; i < toPrint.size(); i++){
				SourceDataInfo sdi = dataframes.get(toPrint.get(i));
				File file = new File(sdi.getPath());
				st1.bind(1, sdi.getFormattedID())
				.bind(2, sdi.getFormattedName())
				.bind(3, file.lastModified());
				st1.step(); 
				st1.reset();
			} 
			db.exec("COMMIT TRANSACTION;"); 
			writeAllTables(db);
			db.dispose();
		}catch(SQLiteException se){
			try{
	            se.printStackTrace(Logger.getInstance().addTError());
	        }catch(Exception e){}
		}
	}

	public void writeAllTables(SQLiteConnection db){

		try{

			for(int i = 0; i < toPrint.size(); i++){
				SourceDataInfo sdi = dataframes.get(toPrint.get(i));
				Logger.getInstance().addT(sdi.getTable());
				//if(sdi.getType().equals("csv") || sdi.getType().equals("sqlite3")){
					writeCSVTable(db,sdi);
				/*}else{
					writeSQLTable(db,sdi);
				}*/
			}

		}catch(SQLiteException se){
			try{
	            se.printStackTrace(Logger.getInstance().addTError());
	        }catch(Exception e){}
		}
	}

	private void writeCSVTable(SQLiteConnection db, SourceDataInfo sdi) throws SQLiteException{

		DataFrame data = sdi.getDataFrame();
		ArrayList<String> headers = data.getHeaders();
		HashMap<String, Boolean> fieldType = sdi.getDataFrame().getFieldType();

		String query = "CREATE TABLE if NOT EXISTS '"+sdi.getFormattedID()+"' ("; 
		String insertQuery = "INSERT INTO '"+sdi.getFormattedID()+"' VALUES (";
		for(int i = 0; i < headers.size(); i++){
			String temp = headers.get(i);
			temp = "'"+temp+"'";
			query += temp;
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

	private void writeSQLTable(SQLiteConnection db, SourceDataInfo sourceData) throws SQLiteException{

		try{

	    	Driver driver = DriverSelector.getDriver(sourceData.getType());
            Class.forName(driver.packageName());
	        Logger.getInstance().add("Connecting to Server...");

	        driver.createConnection(sourceData.getHost(),sourceData.getUsername(),sourceData.getPassword());

	        Table table;
	        if(sourceData.isMerged()){
	        	table = new MergedTable(sourceData.getQuery(), driver);
	        }else{
	        	table = new BasicTable(sourceData.getTable(), driver);
	        }

	        String[] fields = table.getColumnNames();

	        String query = "CREATE TABLE if NOT EXISTS '"+sourceData.getFormattedID()+"' ("; 
			String insertQuery = "INSERT INTO '"+sourceData.getFormattedID()+"' VALUES (";

			HashMap<String, Boolean> types = new HashMap<String, Boolean>();
			for(int i = 0; i < fields.length; i++){
				String temp = "'"+fields[i]+"'";
				query += temp;
				if(table.getStats(fields[i])[0].equals("real")){
					query += " REAL,";
					types.put(fields[i], true);
				}else{
					query += " TEXT,";
					types.put(fields[i], false);
				}
				insertQuery += "?,";
			}

			query = query.substring(0, query.length()-1);
			insertQuery = insertQuery.substring(0, insertQuery.length()-1);
			query += ");";
			insertQuery += ");";

			Logger.getInstance().addT(query+"\n");
			Logger.getInstance().addT(insertQuery);
			
			SQLiteStatement st0 = db.prepare(query);
			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			SQLiteStatement st1 = db.prepare(insertQuery, true);

	        String sql = sourceData.getQuery();
			PreparedStatement pstmt = driver.getConnection().prepareStatement(sql);
			Logger.getInstance().add("Executing query for table, "+sourceData.getTable()+", data...");
            ResultSet rs = pstmt.executeQuery();

            while(rs.next()){

				for(int i = 0; i < fields.length; i++){
					if(types.get(fields[i])){
						st1.bind(i+1, rs.getDouble(fields[i]));
					}else{
						st1.bind(i+1, rs.getString(fields[i]));
					}

				}
				st1.step(); 
				st1.reset();
			}
	        rs.close();
	        db.exec("COMMIT TRANSACTION;"); 
	        driver.getConnection().close();
	    }catch(SQLException se){
	        try{
	            se.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }catch(Exception e){
	        try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }
	}

	private void setToPrint(){

		toPrint = new ArrayList<Integer>();
		for(int i = 0; i < rootIds.size(); i++){
			int temp = templates.get(rootIds.get(i)).getDataSource();
			if(!toPrint.contains(temp)){
				toPrint.add(temp);
			}
		}
	}

}