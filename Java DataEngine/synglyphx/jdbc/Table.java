package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;
import synglyphx.util.Functions;
import synglyphx.jdbc.driver.Driver;

public abstract class Table {
	
	protected String name;
	protected String query;
	protected String end_of_query;
	protected Driver driver;
	protected ArrayList<String> columnNames;
	protected HashMap<String,String> columnTypes;
	protected HashMap<String,DataStats> dataStats;
	protected HashMap<String,String> jdbcTypes;
	protected HashMap<String,ArrayList<String>> min_max_table;

	public Table(Driver driver){
		this.driver = driver;
		initialize();
	}

	private void columnDataStats(String cn){

		try{
			String[] ranges = new String[3];
			String[] counts = new String[2];
			String sql = driver.dataStatsQuery(cn, end_of_query, columnTypes.get(cn).equals("Double"));
			Logger.getInstance().add(sql);
			Statement stmt = driver.getConnection().createStatement();
            ResultSet rs = stmt.executeQuery(sql);

            while(rs.next()){
            	int i = 1;
            	ranges[0] = rs.getString(i++);
            	ranges[1] = rs.getString(i++);
            	if(columnTypes.get(cn).equals("Double")){
            		ranges[2] = rs.getString(i++);
            	}else{
            		ranges[2] = "N/A";
            		i++;
            	}
            	counts[0] = rs.getString(i++);
            	counts[1] = rs.getString(i++);
            	dataStats.put(cn, new DataStats(columnTypes.get(cn),ranges,counts));
            	DataStats ds = dataStats.get(cn);
            	Logger.getInstance().add(ds.getType()+" | "+ds.getMin()+" | "+ds.getMax()+" | "+ds.getAverage()+" | "+ds.getCount()+" | "+ds.getDistinct()); 
  				min_max_table.put(cn, new ArrayList<String>());
  				min_max_table.get(cn).add(ds.getMin());   
  				min_max_table.get(cn).add(ds.getMax());       	     	
            }

            rs.close();
        }catch(SQLException se){
        	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
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
		jdbcTypes.put("TEXT","String");
		jdbcTypes.put("CHAR","String");
		jdbcTypes.put("VARCHAR","String");
		jdbcTypes.put("BIT","Void");
		jdbcTypes.put("BINARY","Void");
		jdbcTypes.put("VARBINARY","Void");
		jdbcTypes.put("TINYBLOB","Void");
		jdbcTypes.put("BLOB","Void");
		jdbcTypes.put("MEDIUMBLOB","Void");
		jdbcTypes.put("LONGBLOB","Void");
		
		jdbcTypes.put("DATE","Date");
		jdbcTypes.put("DATETIME","Timestamp");
		jdbcTypes.put("TIMESTAMP","Timestamp");
		jdbcTypes.put("TIME","Time");
		jdbcTypes.put("YEAR","Date");

		columnNames = new ArrayList<String>();
		columnTypes = new HashMap<String,String>();

	}

	public void createDataStats(){

		Logger.getInstance().add("creating datastats");
		dataStats = new HashMap<String,DataStats>();
		min_max_table = new HashMap<String, ArrayList<String>>();
		for(int i = 0; i < columnNames.size(); i++){
			columnDataStats(columnNames.get(i));
		}

	}

	public String getTableName(){
		return name;
	}

	public HashMap<String, ArrayList<String>> getMinMaxTable(){
		return min_max_table;
	}

	public HashMap<String,DataStats> getDataStats(){
		return dataStats;
	}

}