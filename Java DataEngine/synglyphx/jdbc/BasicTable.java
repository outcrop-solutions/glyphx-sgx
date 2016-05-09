package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;
import synglyphx.util.Functions;
import synglyphx.jdbc.driver.Driver;

public class BasicTable extends Table{
	
	private ArrayList<String[]> sampleData;

	public BasicTable(String name, Driver driver){
		super(driver);
		this.name = name;
		this.query = "SELECT * FROM "+name;
		this.end_of_query = name;
		setColumnNames();
		//createDataStats();
		//loadSampleData();
	}

	public BasicTable(String name, String query, Driver driver){
		super(driver);
		this.name = name;
		this.query = query;
		this.end_of_query = query.split("FROM")[1];
		setColumnNames();
		createDataStats();
	}

	private void setColumnNames(){

		String column_type;
		String column_name;

		try{

			String sql = query+" LIMIT 1";
			PreparedStatement pstmt = driver.getConnection().prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	column_type = metaData.getColumnTypeName(i + 1);
            	column_name = metaData.getColumnName(i + 1);
            	if(jdbcTypes.containsKey(column_type.toUpperCase())){
                	columnNames.add(driver.basicField(metaData.getTableName(i + 1), column_name));
                	columnTypes.put(driver.basicField(metaData.getTableName(i + 1), column_name), jdbcTypes.get(column_type.toUpperCase()));
            	}
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
/*
	private void loadSampleData(){

		sampleData = new ArrayList<String[]>();
		try{

			String sql = "SELECT * FROM "+name+" LIMIT 15";  
			Statement stmt = driver.getConnection().createStatement();
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
*/
}