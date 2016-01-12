package synglyphx.jdbc;

import java.sql.*;
import java.util.HashMap;

public class Database {
	
	private Connection conn;
	private String[] table_names;
	private HashMap<String,Table> tables;

	public Database(Connection conn){
		this.conn = conn;
		tables = new HashMap<String,Table>();
		setTableMetaData();
	}

	public void setTableMetaData(){

		String name;
		int count = 0;
		try{
	        DatabaseMetaData md = conn.getMetaData();
	        ResultSet rs = md.getTables(null, null, "%", null);

	        if(rs.last()){
            	count = rs.getRow();
            	rs.beforeFirst(); 
         	}
         	table_names = new String[count];
         	count = 0;
	        while (rs.next()) {
	        	name = rs.getString(3);
	            table_names[count++] = name;
	        }
	        rs.close();
        }catch(SQLException se){
         	se.printStackTrace();
      	}

	}

	public void initializeChosenTables(String[] chosen){

		try{

			for(int i = 0; i < chosen.length; i++){
	    		tables.put(chosen[i], new Table(chosen[i], conn.createStatement()));
	    	}

	    }catch(SQLException se){
        	se.printStackTrace();
      	}
	}

	public void initializeQueryTables(String query){

		try{

	    	//tables.put(chosen[i], new Table(chosen[i], conn.createStatement()));

	    }catch(SQLException se){
        	se.printStackTrace();
      	}
	}

	public Table getTable(int i){
		return tables.get(table_names[i]);
	}

	public String[] getTableNames(){
		return table_names;
	}

}