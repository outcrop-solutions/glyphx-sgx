package synglyphx.jdbc.driver;

import java.sql.*;
import synglyphx.io.Logger;
import java.util.ArrayList;

public class VerticaDriver implements Driver {

	private Connection conn = null;
	private String conn_str;
	private String un;
	private String pw;
	private String query;

	public String packageName(){
		return "com.vertica.jdbc.Driver";
	}

	public void createConnection(String conn_str, String un, String pw) throws SQLException {
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
		
		String query = "SELECT COUNT("+names.get(0)+"),";
        for(int i = 0; i < names.size(); i++){
            query += "MIN("+names.get(i)+"),MAX("+names.get(i)+"),COUNT(DISTINCT("+names.get(i)+"))";
            if(i != names.size()-1){
               query += ", ";
            }
        }query += " FROM "+eoq;

        return query;
	}

	public String basicField(String tbl_name, String col_name){
		return col_name;
	}

	public String mergedField(String tbl_name, String col_name){
		return tbl_name+"."+col_name;
	}

	public ResultSet getImportedKeys(DatabaseMetaData dm, String name) throws SQLException {
		String[] split = name.split("\\.");
		if(split.length == 1){
			return dm.getImportedKeys(null, null, name);
		}
		return dm.getImportedKeys(null, split[0], split[1]);
	}

	public String getLimit(){
		return " LIMIT 1";
	}

	public void setQuery(String query){
		this.query = query;
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
}