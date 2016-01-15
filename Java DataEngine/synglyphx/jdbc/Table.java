package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;

public class Table {
	
	private String name;
	private Connection conn;
	private ArrayList<String> columnNames;
	private HashMap<String,String> columnTypes;
	private HashMap<String,DataStats> dataStats;
	private HashMap<String,String> jdbcTypes;
	private HashMap<String, ArrayList<String>> min_max_table;

	public Table(String name, Connection conn){
		this.name = name;
		this.conn = conn;
		Logger.getInstance().add("");
		Logger.getInstance().add(name + " DataStats:");
		initialize();
	}

	private void setColumnNames(){

		String column_type;
		String column_name;

		try{

			String sql = "SELECT * FROM "+name;
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

	private void createDataStats(String cn){

		String[] ranges;
		String[] counts;
		min_max_table = new HashMap<String, ArrayList<String>>();

		try{

			ranges = new String[3];
			counts = new String[2];
			String sql = "SELECT MIN(`"+cn+"`),MAX(`"+cn+"`),AVG(`"+cn+"`),COUNT(`"+cn+"`),COUNT(DISTINCT(`"+cn+"`)) FROM "+name;  
			Statement stmt = conn.createStatement();
            ResultSet rs = stmt.executeQuery(sql);

            while(rs.next()){
            	ranges[0] = rs.getString(1);
            	ranges[1] = rs.getString(2);
            	ranges[2] = rs.getString(3);
            	counts[0] = rs.getString(4);
            	counts[1] = rs.getString(5);
            	dataStats.put(cn, new DataStats(columnTypes.get(cn),ranges,counts));
            	DataStats ds = dataStats.get(cn);
            	Logger.getInstance().add(ds.getType()+" | "+ds.getMin()+" | "+ds.getMax()+" | "+ds.getAverage()+" | "+ds.getCount()+" | "+ds.getDistinct()); 
  				min_max_table.put(cn, new ArrayList<String>());
  				min_max_table.get(cn).add(ds.getMin());   
  				min_max_table.get(cn).add(ds.getMax());       	     	
            }

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
		dataStats = new HashMap<String,DataStats>();
		setColumnNames();

		for(int i = 0; i < columnNames.size(); i++){
			createDataStats(columnNames.get(i));
		}
	}

	public HashMap<String, ArrayList<String>> getMinMaxTable(){
		return min_max_table;
	}
}