package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;

public abstract class EmptyTable {
	
	protected String name;
	protected String query;
	protected String end_of_query;
	protected Connection conn;
	protected ArrayList<String> columnNames;
	protected HashMap<String,String> columnTypes;
	protected HashMap<String,DataStats> dataStats;
	protected HashMap<String,String> jdbcTypes;
	protected HashMap<String,ArrayList<String>> min_max_table;

	public EmptyTable(Connection conn){
		this.conn = conn;
		initialize();
	}

	private void columnDataStats(String cn){

		String[] ranges;
		String[] counts;
		String sql;

		try{

			ranges = new String[3];
			counts = new String[2];
			sql = "SELECT MIN(`"+cn+"`),MAX(`"+cn+"`),AVG(`"+cn+"`),";
			sql += "COUNT(`"+cn+"`),COUNT(DISTINCT(`"+cn+"`)) FROM "+end_of_query;
			Logger.getInstance().add(sql);
			Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);

            while(rs.next()){
            	ranges[0] = rs.getString(1);
            	ranges[1] = rs.getString(2);
            	ranges[2] = rs.getString(3);
            	counts[0] = rs.getString(4);
            	counts[1] = rs.getString(5);
            	System.out.println(columnTypes.get(cn));
            	System.out.println(ranges[2]);
            	System.out.println(counts[1]);
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

		String[] fields = new String[columnNames.size()];
		for(int i = 0; i < columnNames.size(); i++){
			fields[i] = columnNames.get(i);
		}
		return fields;
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