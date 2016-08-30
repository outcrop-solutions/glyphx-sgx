package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;
import synglyphx.util.Functions;
import synglyphx.data.DataFrame;
import synglyphx.jdbc.driver.Driver;
import synglyphx.util.ErrorHandler;

public abstract class Table {
	
	protected String name;
	protected String query;
	protected String end_of_query;
	protected Driver driver;
	protected DataFrame data;
	protected ArrayList<String> columnNames;
	protected HashMap<String,String> columnTypes;
	protected HashMap<String,DataStats> dataStats;
	protected HashMap<String,String> jdbcTypes;
	protected HashMap<String,ArrayList<String>> min_max_table;

	public Table(Driver driver){
		this.driver = driver;
		initialize();
	}

	protected void createDataStats(){

		Logger.getInstance().add("creating datastats");
		dataStats = new HashMap<String,DataStats>();
		min_max_table = new HashMap<String, ArrayList<String>>();
		try{
			String[] ranges = new String[3];
			String[] counts = new String[2];
			//String sql = driver.dataStatsQuery(cn, end_of_query, columnTypes.get(cn).equals("Double"));
			String sql = driver.dataStatsQuery(columnNames, end_of_query);
			Statement stmt = driver.getConnection().createStatement();
			//System.out.println(sql);
            ResultSet rs = stmt.executeQuery(sql);
            /*
            while(rs.next()){
            	ranges[0] = rs.getString(1);
            	ranges[1] = rs.getString(2);
            	ranges[2] = "-";
            	counts[0] = rs.getString(3);
            	counts[1] = rs.getString(4);
            	dataStats.put(cn, new DataStats(columnTypes.get(cn),ranges,counts));
            	DataStats ds = dataStats.get(cn);
            	Logger.getInstance().add(ds.getType()+" | "+ds.getMin()+" | "+ds.getMax()+" | "+ds.getAverage()+" | "+ds.getCount()+" | "+ds.getDistinct()); 
  				min_max_table.put(cn, new ArrayList<String>());
  				min_max_table.get(cn).add(ds.getMin());   
  				min_max_table.get(cn).add(ds.getMax());       	     	
            }*/

            rs.next();

            ranges[2] = "-";
            counts[0] = rs.getString(1);
            int cc = 0;
	        for(int i = 1; i < columnNames.size()*3; i+=3){
	            ranges[0] = rs.getString(i+1);
	            ranges[1] = rs.getString(i+2);
	            counts[1] = rs.getString(i+3);
	            String temp = columnNames.get(cc);
	            dataStats.put(temp, new DataStats(columnTypes.get(temp),ranges,counts));
            	DataStats ds = dataStats.get(temp);
	            Logger.getInstance().add(ds.getType()+" | "+ds.getMin()+" | "+ds.getMax()+" | "+ds.getAverage()+" | "+ds.getCount()+" | "+ds.getDistinct()); 
  				min_max_table.put(temp, new ArrayList<String>());
  				min_max_table.get(temp).add(ds.getMin());   
  				min_max_table.get(temp).add(ds.getMax());  
  				cc += 1;
	        }

            rs.close();
        }catch(Exception e){
        	try{
            	e.printStackTrace(ErrorHandler.getInstance().addError());
         	}catch(Exception ex){}
         	e.printStackTrace();
        }
	}

	public String[] getColumnNames(){
		return Functions.arrayListToStringList(columnNames);
	}

	public String[] getStats(String field){

		String[] stats = new String[6];
		DataStats ds = dataStats.get(field);
		stats[0] = ds.getType();
		stats[1] = ds.getMin();
		stats[2] = ds.getMax();
		stats[3] = ds.getAverage();
		stats[4] = ds.getCount();
		stats[5] = ds.getDistinct();
		return stats;
	}

	private void initialize(){

		jdbcTypes = new HashMap<String,String>();
		jdbcTypes.put("TINYINT","Double");
		jdbcTypes.put("SMALLINT","Double");
		jdbcTypes.put("MEDIUMINT","Double");
		jdbcTypes.put("INT","Double");
		jdbcTypes.put("INTEGER","Double");
		jdbcTypes.put("BIGINT","Double");
		jdbcTypes.put("FLOAT","Double");
		jdbcTypes.put("DOUBLE","Double");
		jdbcTypes.put("DECIMAL","Double");
		jdbcTypes.put("REAL","Double");
		jdbcTypes.put("NUMERIC","Double");
		jdbcTypes.put("LONG","Double");
		jdbcTypes.put("NUMBER","Double");

		jdbcTypes.put("TEXT","String");
		jdbcTypes.put("CHAR","String");
		jdbcTypes.put("VARCHAR","String");
		jdbcTypes.put("VARCHAR2","String");
		jdbcTypes.put("BIT","String");
		jdbcTypes.put("BINARY","String");
		jdbcTypes.put("VARBINARY","String");
		jdbcTypes.put("TINYBLOB","String");
		jdbcTypes.put("BLOB","String");
		jdbcTypes.put("MEDIUMBLOB","String");
		jdbcTypes.put("LONGBLOB","String");
		
		jdbcTypes.put("DATE","String");
		jdbcTypes.put("DATETIME","String");
		jdbcTypes.put("TIMESTAMP","String");
		jdbcTypes.put("TIME","String");
		jdbcTypes.put("YEAR","String");
		jdbcTypes.put("TIMETZ","String");
		jdbcTypes.put("TIMESTAMPTZ","String");
		jdbcTypes.put("INTERVAL YEAR TO MONTH","String");
		jdbcTypes.put("INTERVAL HOUR TO SECOND","String");
		jdbcTypes.put("INTERVAL HOUR TO MINUTE","String");
		jdbcTypes.put("INTERVAL DAY TO SECOND","String");

		columnNames = new ArrayList<String>();
		columnTypes = new HashMap<String,String>();

	}
/*
	public void createDataStats(){

		Logger.getInstance().add("creating datastats");
		dataStats = new HashMap<String,DataStats>();
		min_max_table = new HashMap<String, ArrayList<String>>();
		for(int i = 0; i < columnNames.size(); i++){
			columnDataStats(columnNames.get(i));
		}
	}
*/
	public String getTableName(){
		return name;
	}

	public HashMap<String, ArrayList<String>> getMinMaxTable(){
		return min_max_table;
	}

	public HashMap<String,DataStats> getDataStats(){
		return dataStats;
	}

	public DataFrame createDataFrame(){

		this.data = new DataFrame();
		this.data.addRow(columnNames);

		try{
			PreparedStatement pstmt = driver.getConnection().prepareStatement(this.query);
	        ResultSet rs = pstmt.executeQuery();

	        while(rs.next()){
	        	ArrayList<String> row = new ArrayList<String>();
	        	for(int i = 0; i < columnNames.size(); i++){
	        		String temp = rs.getString(columnNames.get(i));
	        		if(temp == null){temp = "";}
	        		row.add(temp);
	        	}   	     	
	  			this.data.addRow(row);
	        }
		    rs.close();
		    driver.getConnection().close();
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }

	    HashMap<String, ArrayList<String>> minMaxTable = new HashMap<String, ArrayList<String>>();
	    HashMap<String, Boolean> fieldType = new HashMap<String, Boolean>();
	    for(int i = 0; i < columnNames.size(); i++){
	    	minMaxTable.put(columnNames.get(i), new ArrayList<String>());
	    	String[] stats = getStats(columnNames.get(i));
	    	minMaxTable.get(columnNames.get(i)).add(stats[1]);
	    	minMaxTable.get(columnNames.get(i)).add(stats[2]);
	    	if(stats[0].equals("real")){
	    		fieldType.put(columnNames.get(i), true);
	    	}else{
	    		fieldType.put(columnNames.get(i), false);
	    	}
	    }	
	    data.setMinMaxTable(minMaxTable);
	    data.setFieldTypes(fieldType);

		return data;
	}

	public DataFrame getDataFrame(){
		return data;
	}

}