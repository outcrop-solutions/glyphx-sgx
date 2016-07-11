package synglyphx.io;

import java.io.*;
import java.sql.*;
import java.util.Arrays;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.data.DataFrame;
import synglyphx.data.SourceDataInfo;
import synglyphx.util.ErrorHandler;

public class SQLiteReader {

	private DataFrame data = null;
	private ArrayList<String> tableNames = null;
	private int headers = 0;

	public SQLiteReader(){}

	public static boolean isAntzUpdateNeeded(String ts, String dir, ArrayList<SourceDataInfo> dataPaths){

		File f = new File(dir+"/sourcedata.db");
		if(!f.exists()){return true;}

		try{

			Class.forName("org.sqlite.JDBC");
			String conn_str = "jdbc:sqlite:"+dir+"/sourcedata.db";
			Connection conn = DriverManager.getConnection(conn_str);
		    String query = "SELECT lastChanged FROM SDTInfo;";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        rs.next();
			String lastChanged = rs.getString("lastChanged");
			rs.close();
			pstmt.close();

			if(checkModified(dataPaths, conn))
				return true;

			if(Double.parseDouble(ts) <= Double.parseDouble(lastChanged)){ 
				System.out.println("No update needed");
				Logger.getInstance().add("No update needed");
				conn.close();
				return false; 
			}
			conn.close();
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }

		return true;
	}

	private static boolean checkModified(ArrayList<SourceDataInfo> dataPaths, Connection conn) {

		try{
			
			String query = "SELECT FormattedName, Timestamp FROM 'TableIndex';";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

			HashMap<String, String> timestamps = new HashMap<String, String>();
			while(rs.next()){
				timestamps.put(rs.getString("FormattedName"),rs.getString("Timestamp"));
				Logger.getInstance().add("CM: "+rs.getString("FormattedName"));
			}
			rs.close();
			pstmt.close();
			conn.close();
			for(int i = 0; i < dataPaths.size(); i++){
				Logger.getInstance().add("CM: "+dataPaths.get(i).getFormattedName());
				if(timestamps.containsKey(dataPaths.get(i).getFormattedName())){
					File file = new File(dataPaths.get(i).getPath());
					Logger.getInstance().add(dataPaths.get(i).getPath());
					if(file.lastModified() > Double.parseDouble(timestamps.get(dataPaths.get(i).getFormattedName()))){
						return true;
					}
				}
			}
			return false;

		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	    return true;
	}
/*
	public static void main(String [] args){
		SQLiteReader reader = new SQLiteReader();
		reader.createDataFrame("temp.sqlite");
	}*/
}