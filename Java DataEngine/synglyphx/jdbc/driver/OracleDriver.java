package synglyphx.jdbc.driver;

import java.sql.*;
import synglyphx.io.Logger;
import java.util.ArrayList;
import java.util.HashMap;

public class OracleDriver implements Driver {

	private Connection conn = null;
	private String conn_str;
	private String un;
	private String pw;
	private String query;
	private ArrayList<String> tbls;
	private ArrayList<Integer> used;
	private HashMap<String, ArrayList<String>> tbl_columns;

	public String packageName(){
		return "oracle.jdbc.driver.OracleDriver";
	}

	public void createConnection(String conn_str, String un, String pw) throws SQLException {
		String[] strsplt = conn_str.split("oracle:");
		conn_str = strsplt[0]+"oracle:thin:"+strsplt[1];
		Logger.getInstance().add("Creating connection...");
		Logger.getInstance().add(conn_str);
		Logger.getInstance().add(un);
		Logger.getInstance().add(pw);
		this.conn_str = conn_str;
		this.un = un;
		this.pw = pw;
		try{
			this.conn = DriverManager.getConnection(conn_str,un,pw);
		}catch(SQLException se){
	        try{
	            se.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
      	}catch(Exception e){
         	try{
            	e.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}

	public String dataStatsQuery(String cn, String eoq, boolean num){

		String sql = "SELECT MIN("+cn+"),MAX("+cn+"),";
		sql += "COUNT("+cn+"),COUNT(DISTINCT("+cn+")) FROM "+eoq;
		return sql;
	}

	public String dataStatsQuery(ArrayList<String> names, String eoq){
		
		ArrayList<String> temp = new ArrayList<String>();
		String query = "SELECT COUNT("+names.get(0)+"),";
        for(int i = 0; i < names.size(); i++){
        	if(!temp.contains(names.get(i))){
        		if(i != 0){
	               query += ", ";
	            }
	            query += "MIN("+names.get(i)+"),MAX("+names.get(i)+"),COUNT(DISTINCT("+names.get(i)+"))";
	            temp.add(names.get(i));
	        }
        }query += " FROM "+eoq;
        //System.out.println(query);

        return query;
	}

	public String basicField(String tbl_name, String col_name){
		return col_name;
	}

	public String mergedField(String tbl_name, String col_name){

		int index = 0;
		boolean found = false;
		if(tbl_name.equals("")){
			for(int i = 0; i < tbls.size(); i++){
				ArrayList<String> temp = tbl_columns.get(tbls.get(i));
				for(int j = 0; j < temp.size(); j++){
					index += 1;
					if(temp.get(j).equals(col_name) && !used.contains(index)){
						tbl_name = tbls.get(i);
						used.add(index);
						found = true;
						break;
					}
				}
				if(found){break;}
			}
		}
		//System.out.println(tbl_name+"."+col_name);
		return tbl_name+"."+col_name;
	}

	public ResultSet getImportedKeys(DatabaseMetaData dm, String name) throws SQLException {
		String[] split = name.split("\\.");
		if(split.length == 1){
			return dm.getImportedKeys(null, null, name);
		}
		return dm.getImportedKeys(null, split[0], split[1]);
	}

	public Connection getNewConnection(){
		try{
			return DriverManager.getConnection(conn_str,un,pw);
		}catch(SQLException se){
	        try{
	            se.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
      	}catch(Exception e){
         	try{
            	e.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
      	return null;
	}

	public Connection createTempConnection(String conn_str, String un, String pw) throws SQLException {
		return DriverManager.getConnection(conn_str,un,pw);
	}

	public Connection getConnection(){
		return conn;
	}

	public String getFormattedName(String name){
		String[] split = name.split("\\.");
		if(split.length > 1){
			return split[1];
		}
		return name;
	}

	public void setQuery(String query){
		this.query = query;
		String base_tbl = query.split("FROM \\(")[1].split(" INNER JOIN")[0];
		String[] joins = query.split("INNER JOIN ");
		tbls = new ArrayList<String>();
		tbls.add(base_tbl);
		tbl_columns = new HashMap<String, ArrayList<String>>();
		tbl_columns.put(base_tbl, new ArrayList<String>());
		populateColumns(base_tbl);
		for(int i = 1; i < joins.length; i++){
			String temp = joins[i].split(" ON ")[0];
			tbls.add(temp);
			tbl_columns.put(temp, new ArrayList<String>());
			populateColumns(temp);
		}
		used = new ArrayList<Integer>();
	}

	public String getLimit(){
		return " WHERE ROWNUM = 1";
	}

	private void populateColumns(String tbl){
		try{
			String sql = "SELECT * FROM "+tbl+" WHERE 1 = 0";
			PreparedStatement pstmt = conn.prepareStatement(sql);
            ResultSet rs = pstmt.executeQuery();
            ResultSetMetaData metaData = rs.getMetaData();

            int rowCount = metaData.getColumnCount();

            for (int i = 0; i < rowCount; i++) {
            	String column_name = metaData.getColumnName(i + 1);
            	tbl_columns.get(tbl).add(column_name);
            	//System.out.println(tbl+"."+column_name);
            }
            rs.close();
            //System.out.println(tbl_columns.get(tbl).size());
		}catch(SQLException se){
         	try{
            	se.printStackTrace(Logger.getInstance().addError());
         	}catch(Exception ex){}
      	}
	}
}