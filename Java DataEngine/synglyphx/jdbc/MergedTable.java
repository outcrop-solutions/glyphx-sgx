package synglyphx.jdbc;

import java.sql.*;
import java.util.ArrayList;
import java.util.HashMap;
import synglyphx.io.Logger;

public class MergedTable {

	private Connection conn;
	private String query;
	private ArrayList<String> tables;
	private String[] column_names;
	private HashMap<String,Table> table_map;
	private HashMap<String,String[]> column_stats;
	private HashMap<String,ArrayList<String>> tables_columns;

	public MergedTable(Connection conn, String query){
		this.conn = conn;
		this.query = query;
		tables = new ArrayList<String>();
		table_map = new HashMap<String,Table>();
		column_stats = new HashMap<String,String[]>();
		tables_columns = new HashMap<String,ArrayList<String>>();
		setColumnNames();
	}

	private void setColumnNames(){

		String table_name;
		String column_name;

		try{

			String sql = query;
			PreparedStatement pstmt = conn.prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	table_name = metaData.getTableName(i + 1);
            	column_name = metaData.getColumnName(i + 1);
            	if(!tables_columns.containsKey(table_name)){
            		tables_columns.put(table_name, new ArrayList<String>());
            		tables.add(table_name);
            	}
            	tables_columns.get(table_name).add(column_name);

            }
            rs.close();

		}catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}

	public String[] getColumnNames(){
		return column_names;
	}

	public String[] getRequiredTables(){

		String[] temp = new String[tables.size()];
		for(int i = 0; i < tables.size(); i++){
			temp[i] = tables.get(i);
		}
		return temp;
	}

	public void addTableData(String name, Table table){
		if(table == null){System.out.println("its null");}
		table_map.put(name, table);
	}

	public void setStats(){

		ArrayList<String> all_cols = new ArrayList<String>();
		for(int i = 0; i < tables.size(); i++){
			Table temp = table_map.get(tables.get(i));
			ArrayList<String> temp_cols = tables_columns.get(tables.get(i));
			for(int j = 0; j < temp_cols.size(); j++){
				System.out.println(temp_cols.get(j));
				column_stats.put(temp.getTableName()+":"+temp_cols.get(j), temp.getStats(temp_cols.get(j)));
				all_cols.add(temp.getTableName()+":"+temp_cols.get(j));
			}
		}
		column_names = new String[all_cols.size()];
		for(int i = 0; i < all_cols.size(); i++){
			column_names[i] = all_cols.get(i);
		}
	}

	public String[] getStats(String field){
		return column_stats.get(field);
	}
}