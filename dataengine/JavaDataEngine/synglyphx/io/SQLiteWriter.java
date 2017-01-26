package synglyphx.io;

import java.sql.*;
import java.util.Date;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.File;
import synglyphx.jdbc.Table;
import synglyphx.jdbc.BasicTable;
import synglyphx.jdbc.MergedTable;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;
import synglyphx.data.DataFrame;
import synglyphx.data.SourceDataInfo;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.util.ErrorHandler;

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
	private Connection conn = null;

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
			Class.forName("org.sqlite.JDBC");
			String conn_str = "jdbc:sqlite:"+outDir+"/sourcedata.db";
			//String conn_str = "jdbc:sqlite::memory:";
			conn = DriverManager.getConnection(conn_str);

			PreparedStatement pstmt = conn.prepareStatement("PRAGMA journal_mode = OFF;");
			pstmt.execute();
			pstmt.close();
/*
			String query = "CREATE TABLE if NOT EXISTS 'SDTInfo' (lastChanged TEXT)";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        boolean created = pstmt.execute();

	        query = "INSERT INTO 'SDTInfo' VALUES ("+timestamp+")";
	        pstmt = conn.prepareStatement(query);
	        boolean inserted = pstmt.execute();
	        pstmt.close();
*/
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	public void writeTableIndex(){

		try{
			String query = "CREATE TABLE if NOT EXISTS TableIndex "; 
			query += "(TableName TEXT, FormattedName TEXT, Timestamp INTEGER);";
			Statement stmt = conn.createStatement();
	        boolean created = stmt.execute(query);
	        stmt.close();

	        ArrayList<String> written = new ArrayList<String>();
			PreparedStatement pstmt = conn.prepareStatement("INSERT INTO TableIndex VALUES(?,?,?);");
			for(int i = 0; i < toPrint.size(); i++){
				SourceDataInfo sdi = dataframes.get(toPrint.get(i));
				File file = new File(sdi.getPath());
				if(!written.contains(sdi.getFormattedID())){
					pstmt.setString(1, sdi.getFormattedID());
					pstmt.setString(2, sdi.getFormattedName());
					pstmt.setLong(3, file.lastModified());
					pstmt.addBatch();
				}
				written.add(sdi.getFormattedID());
			} 
			int[] updates = pstmt.executeBatch();
			pstmt.close();
			writeAllTables();
			conn.close();
			ErrorHandler.getInstance().sqliteWriterCompleted();
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	public void writeAllTables(){

		try{
			conn.setAutoCommit(false);
			ArrayList<String> written = new ArrayList<String>();
			for(int i = 0; i < toPrint.size(); i++){
				SourceDataInfo sdi = dataframes.get(toPrint.get(i));
				if(!written.contains(sdi.getFormattedID())){
					Logger.getInstance().addT(sdi.getTable());
					writeTable(sdi);
				}
				written.add(sdi.getFormattedID());
			}

		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	private void writeTable(SourceDataInfo sdi) throws SQLException{

		DataFrame data = sdi.getDataFrame();
		ArrayList<String> headers = data.getHeaders();
		HashMap<String, Boolean> fieldType = sdi.getDataFrame().getFieldType();

		String query = "CREATE TABLE if NOT EXISTS '"+sdi.getFormattedID()+"' ("; 
		String insertQuery = "INSERT INTO '"+sdi.getFormattedID()+"' VALUES (";
		for(int i = 0; i < headers.size(); i++){
			query += "'"+headers.get(i).replace("'","''")+"'";
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
		
		PreparedStatement pstmt = conn.prepareStatement(query);
	    boolean created = pstmt.execute();

	    pstmt = conn.prepareStatement(insertQuery);

		int size = data.size();
		for(int i = 0; i < size; i++){
			ArrayList<String> row = data.getRow(i);
			for(int j = 0; j < row.size(); j++){
				if(row.get(j).equals("")){
					pstmt.setString(j+1, Character.toString((char)128));
				}else{
					pstmt.setString(j+1, row.get(j));
				}
			}
			pstmt.addBatch();
			if((i+1) % 5000 == 0){
				int[] updates = pstmt.executeBatch();
				conn.commit();
			}
		}
		int[] updates = pstmt.executeBatch();
		pstmt.close();
		conn.commit();
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