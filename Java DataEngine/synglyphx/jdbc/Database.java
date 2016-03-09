package synglyphx.jdbc;

import java.sql.*;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.util.Functions;
import synglyphx.io.Logger;
import synglyphx.jdbc.driver.Driver;

public class Database {
	
	private Driver driver;
	private String[] table_names;
	private HashMap<String,BasicTable> tables;
	private HashMap<String,BasicTable> temp_tables;
	private String base_table;
	private MergedTable mergedTable;
	private ArrayList<String> schemas;
	private HashMap<String,ArrayList<String>> tables_by_schema;
	private Thread thread;
	private String schema = null;

	public Database(Driver driver){
		this.driver = driver;
		tables = new HashMap<String,BasicTable>();
		temp_tables = new HashMap<String,BasicTable>();
		schemas = new ArrayList<String>();
		setTableMetaData();
	}

	public void setTableMetaData(){

		String name;
		int count = 0;
		try{
	        DatabaseMetaData md = driver.getConnection().getMetaData();
	        ResultSet sch = md.getSchemas();
	        Logger.getInstance().add("Returned list of schemas");
	        ArrayList<String> temp = new ArrayList<String>();
	        tables_by_schema = new HashMap<String,ArrayList<String>>();
	        while(sch.next()){
	        	ResultSet priv = md.getTablePrivileges(null,sch.getString(1),null);
	        	while(priv.next()){
	        		String schm = priv.getString(2);
	
		        	if(!schemas.contains(schm)){
		        		schemas.add(schm);
		        		tables_by_schema.put(schm, new ArrayList<String>());
		        	}
		        	temp.add(schm+"."+priv.getString(3));
		        	tables_by_schema.get(schm).add(schm+"."+priv.getString(3));
		
	        	}
	        	priv.close();
	        }
	        sch.close();
			Logger.getInstance().add("Returned "+String.valueOf(schemas.size())+" schemas");
			if(schemas.size() == 0){
		        ResultSet rs = md.getTables(null, null, "%", null);
		        while (rs.next()) {
		        	name = rs.getString(3);
		        	if(!name.equals("sqlite_sequence")){
		            	temp.add(name);
		            }
		        }
		        rs.close();
		    }
	     
	        table_names = new String[temp.size()];
	        for(int i = 0; i < temp.size(); i++){
	        	table_names[i] = temp.get(i);
	        	Logger.getInstance().add(temp.get(i));
	        }

	        Logger.getInstance().add("Creating basic tables");
	        if(schemas.size() == 0){
		        thread = new Thread(){
	    			public void run(){
				        for(int i = 0; i < table_names.length; i++){
				        	temp_tables.put(table_names[i], new BasicTable(table_names[i], driver));
				        }
				    }
				};
				thread.start();
			}

        }catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}

	public void initializeChosenTables(String[] chosen){

		table_names = chosen;
		for(int i = 0; i < chosen.length; i++){
			Logger.getInstance().add(chosen[i]);
	    	tables.put(chosen[i], temp_tables.get(chosen[i]));
	    }
	}

	public void initializeQueryTables(String query, Driver drive){
	    mergedTable = new MergedTable(query, drive);
	}

	public void setBaseTable(String base_table){
		this.base_table = base_table;
	}

	public BasicTable getTable(int i){
		try{
			thread.join();
		}catch(InterruptedException ie){
			ie.printStackTrace();
		}
		Logger.getInstance().add(String.valueOf(i));
		Logger.getInstance().add(table_names[i]);
		return temp_tables.get(table_names[i]);
	}

	public MergedTable getMergedTable(){
		return mergedTable;
	}

	public String[] getTableNames(){
		return table_names;
	}

	public String[] getSchemaTableNames(final String sch){

		if(!sch.equals(schema)){
			this.schema = sch;
			Logger.getInstance().addT(sch);
			thread = new Thread(){
	    		public void run(){
	    			ArrayList<String> temp = tables_by_schema.get(sch);
	    			for(int i = 0; i < temp.size(); i++){
						temp_tables.put(temp.get(i), new BasicTable(temp.get(i), driver));
					}
	    		}
	  		};
	  		thread.start();
	  	}
      	return Functions.arrayListToStringList(tables_by_schema.get(sch));
    }

	public String[] getForeignKeys(String tableName){
		try{
			thread.join();
		}catch(InterruptedException ie){
			ie.printStackTrace();
		}
	    return temp_tables.get(tableName).getForeignKeys();
	}

	public String[] getSampleData(int table, int row){
		//thread.join();
      	return temp_tables.get(table_names[table]).getSampleData(row);
    }

    public String[] getSchemas(){
    	return Functions.arrayListToStringList(schemas);
    }

    public int sizeOfQuery(String query){

    	int size = 0;
    	try{
			String sql = "SELECT count(*) FROM"+query.split("FROM")[1];
			//System.out.println(sql);  
			Statement stmt = driver.getConnection().createStatement();
            ResultSet rs = stmt.executeQuery(sql);
            while(rs.next()){
            	size = rs.getInt(1);
            }
            rs.close();
        }catch(SQLException se){
        	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
         	se.printStackTrace();
        }
        return size;
    }
}