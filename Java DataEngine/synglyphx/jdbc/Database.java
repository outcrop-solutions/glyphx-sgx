package synglyphx.jdbc;

import java.sql.*;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.io.Logger;

public class Database {
	
	private Connection conn;
	private String[] table_names;
	private HashMap<String,Table> tables;
	private HashMap<String,Table> temp_tables;
	private String base_table;

	public Database(Connection conn){
		this.conn = conn;
		tables = new HashMap<String,Table>();
		temp_tables = new HashMap<String,Table>();
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
	        	Logger.getInstance().add(temp.get(i));
	        	temp_tables.put(table_names[i], new Table(temp.get(i), conn));
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
			Logger.getInstance().add(chosen[i]);
	    	tables.put(chosen[i], temp_tables.get(chosen[i]));
	    	tables.get(chosen[i]).createDataStats();
	    }
	}

	public void initializeQueryTables(String query){
/*
	    tables.put(chosen[i], new Table(chosen[i], conn));
*/
	}

	public void setBaseTable(String base_table){
		this.base_table = base_table;
	}

	public Table getTable(int i){
		return temp_tables.get(table_names[i]);
	}

	public String[] getTableNames(){
		return table_names;
	}

	public String[] getForeignKeys(String tableName){
	    return temp_tables.get(tableName).getForeignKeys();
	}

	public String[] getSampleData(int table, int row){
      	return temp_tables.get(table_names[table]).getSampleData(row);
   }

}