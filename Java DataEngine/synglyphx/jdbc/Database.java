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
	private HashMap<String,ArrayList<String>> foreign_key_map; 
	private Thread thread;
	private String schema = null;
	private ArrayList<String> queuedName;
	private ArrayList<String> queuedQuery;

	public Database(Driver driver){
		this.driver = driver;
		tables = new HashMap<String,BasicTable>();
		temp_tables = new HashMap<String,BasicTable>();
		schemas = new ArrayList<String>();
		queuedName = new ArrayList<String>();
		queuedQuery = new ArrayList<String>();
		setTableMetaData();
		if(!driver.packageName().equals("org.sqlite.JDBC"))
			mapForeignKeys();
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
				        	temp_tables.get(table_names[i]).createDataStats();
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

		try{
			thread.join();
		}catch(Exception e){}
		
		table_names = chosen;
		for(int i = 0; i < chosen.length; i++){
			Logger.getInstance().add(chosen[i]);
	    	tables.put(chosen[i], temp_tables.get(chosen[i]));
	    	tables.get(chosen[i]).createDataStats();
	    }
	}

	public void initializeQueryTables(String query, Driver drive){
	    mergedTable = new MergedTable(query, drive);
	}

	public void queueATable(String name, String query){
    	queuedName.add(name);
    	queuedQuery.add(query);
    }

    public void removeQueuedTable(String name){
    	int a = queuedName.indexOf(name);
    	queuedName.remove(a);
    	queuedQuery.remove(a);
    }

    public void executeQueuedTables(){

    	String[] temp_names = new String[queuedName.size()];
    	for(int i = 0; i < queuedName.size(); i++){
    		String n = queuedName.get(i);
    		tables.put(n, new BasicTable(n, queuedQuery.get(i), driver));
    		temp_names[i] = n;
        }
        table_names = temp_names;
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
		//Logger.getInstance().add(String.valueOf(i));
		//Logger.getInstance().add(table_names[i]);
		return tables.get(table_names[i]);
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
		if(foreign_key_map.containsKey(tableName)){
	    	return Functions.arrayListToStringList(foreign_key_map.get(tableName));
	    }
	    return new String[1];
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

    private void mapForeignKeys(){

		foreign_key_map = new HashMap<String,ArrayList<String>>();
		try{

			DatabaseMetaData dm = driver.getConnection().getMetaData();
	    	ResultSet rs = dm.getImportedKeys(null, null, null);;

	    	while (rs.next()) {
	    		String colName = rs.getString(8);
	    		String schtbl = rs.getString(2);
	    		String tblName = rs.getString(3);
	    		String orgName = rs.getString(4);
	    		String fkschtbl = rs.getString(6);
	    		String fktblName = rs.getString(7);
	    		if(schtbl != null){
	    			tblName = schtbl+"."+tblName;
	    		}
	    		if(fkschtbl != null){
	    			fktblName = fkschtbl+"."+fktblName;
	    		}
	    		if(!foreign_key_map.containsKey(fktblName)){
	    			foreign_key_map.put(fktblName, new ArrayList<String>());
	    		}
	    		foreign_key_map.get(fktblName).add(colName);
	    		foreign_key_map.get(fktblName).add(tblName);
	    		foreign_key_map.get(fktblName).add(orgName);
	    	}

	    	rs.close();
	    }catch(SQLException se){
	    	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
	    }
	}
}