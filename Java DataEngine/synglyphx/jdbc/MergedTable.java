package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import synglyphx.io.Logger;
import synglyphx.jdbc.driver.Driver;

public class MergedTable extends Table{

	public MergedTable(String query, Driver driver){
		super(driver);
		this.name = "Merged";
		this.query = query;
		this.end_of_query = query.split("FROM")[1];
		setColumnNames();
		createDataStats();
	}

	private void setColumnNames(){

		String table_name;
		String column_type;
		String column_name;

		try{

			String sql = query;
			driver.setQuery(query);
			PreparedStatement pstmt = driver.getConnection().prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	table_name = metaData.getTableName(i + 1);
            	column_type = metaData.getColumnTypeName(i + 1);
            	column_name = metaData.getColumnName(i + 1);
            	//System.out.println(column_type);
            	String temp = driver.mergedField(table_name, column_name);
            	if(jdbcTypes.containsKey(column_type.toUpperCase())){
            		columnNames.add(temp);
            		columnTypes.put(temp, jdbcTypes.get(column_type.toUpperCase()));
            	}else{
            		columnNames.add(temp);
            		columnTypes.put(temp, "String");
            	}
            }
            rs.close();

		}catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}
}

