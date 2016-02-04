package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;

public class Table extends EmptyTable{
	
	private ArrayList<String[]> sampleData;
	private ArrayList<String> foreign_key_list;
	private HashMap<String,ArrayList<String>> foreign_key_map; 

	public Table(String name, Connection conn){
		super(conn);
		this.name = name;
		this.query = "SELECT * FROM "+name;
		this.end_of_query = name;
		mapForeignKeys();
		setColumnNames();
		loadSampleData();
	}

	private void setColumnNames(){

		String column_type;
		String column_name;

		try{

			String sql = query;
			PreparedStatement pstmt = conn.prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	column_type = metaData.getColumnTypeName(i + 1);
            	column_name = metaData.getColumnName(i + 1);
                columnNames.add(column_name);
                columnTypes.put(column_name, jdbcTypes.get(column_type));
            }
            rs.close();

		}catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}

	public String[] getSampleData(int row){
		return sampleData.get(row);
	}

	public String[] getForeignKeys(){
		String[] temp = new String[foreign_key_list.size()];
		for(int i = 0; i < foreign_key_list.size(); i++){
			temp[i] = foreign_key_list.get(i);
		}
		return temp;
	}

	private void mapForeignKeys(){

		foreign_key_list = new ArrayList<String>();
		foreign_key_map = new HashMap<String,ArrayList<String>>();
		try{
			DatabaseMetaData dm = conn.getMetaData();
	    	ResultSet rs = dm.getImportedKeys(null, null, name);

	    	while (rs.next()) {
	    		String colName = rs.getString(8);
	    		String tblName = rs.getString(3);
	    		String orgName = rs.getString(4);
	    		foreign_key_map.put(colName, new ArrayList<String>());
	    		foreign_key_map.get(colName).add(tblName);
	    		foreign_key_map.get(colName).add(orgName);
	    		foreign_key_list.add(colName);
	    		foreign_key_list.add(tblName);
	    		foreign_key_list.add(orgName);
	    	}

	    	rs.close();
	    }catch(SQLException se){
	    	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
	    }
	}

	private void loadSampleData(){

		sampleData = new ArrayList<String[]>();
		try{

			String sql = "SELECT * FROM "+name+" LIMIT 15";  
			Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);

            int place = 0;
            while(rs.next()){
            	sampleData.add(new String[columnNames.size()]);
            	for(int i = 0; i < columnNames.size(); i++){
            		String temp = rs.getString(columnNames.get(i));
            		if(temp == null){temp = "";}
            		sampleData.get(place)[i] = temp;
            	}   	     	
            	place += 1;
            }

            rs.close();
        }catch(SQLException se){
        	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
        }
	}

}