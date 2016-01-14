package synglyphx.jdbc;

import java.sql.*;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.io.Logger;

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

         	ArrayList<String> temp = new ArrayList<String>();
	        while (rs.next()) {
	        	name = rs.getString(3);
	        	if(!name.equals("sqlite_sequence")){
	            	temp.add(name);
	            }
	        }
	        table_names = new String[temp.size()];
	        for(int i = 0; i < temp.size(); i++){
	        	table_names[i] = temp.get(i);
	        }

	        rs.close();
        }catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}

	}

	public void initializeChosenTables(String[] chosen){

		for(int i = 0; i < chosen.length; i++){
	    	tables.put(chosen[i], new Table(chosen[i], conn));
	    }
	}

	public void initializeQueryTables(String query){
/*
	    tables.put(chosen[i], new Table(chosen[i], conn));
*/
	}

	public Table getTable(int i){
		return tables.get(table_names[i]);
	}

	public String[] getTableNames(){
		return table_names;
	}

}