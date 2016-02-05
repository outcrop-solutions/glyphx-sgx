package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import synglyphx.io.Logger;

public class MergedTable extends EmptyTable{

	public MergedTable(String query, Connection conn){
		super(conn);
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
			PreparedStatement pstmt = conn.prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	table_name = metaData.getTableName(i + 1);
            	column_type = metaData.getColumnTypeName(i + 1);
            	column_name = metaData.getColumnName(i + 1);
            	columnNames.add(table_name+"."+column_name);
            	columnTypes.put(table_name+"."+column_name, jdbcTypes.get(column_type));
            }
            rs.close();

		}catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}
}