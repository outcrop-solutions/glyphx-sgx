package synglyphx.jdbc.driver;

import java.sql.*;
import synglyphx.io.Logger;
import java.util.ArrayList;

public class SQLiteDriver implements Driver {

	private Connection conn = null;
	private String conn_str;
	private String query;

	public String packageName(){
		return "org.sqlite.JDBC";
	}

	public void createConnection(String con_str, String un, String pw) throws SQLException {
		String[] splt_str = con_str.split("jdbc:");
		if(con_str.contains("sqlite:")){
			this.conn_str = con_str;
		}else{
			this.conn_str = "jdbc:sqlite:"+splt_str[1];
		}
		Logger.getInstance().addT(conn_str);
		this.conn = DriverManager.getConnection(conn_str);
	}

	public String dataStatsQuery(String cn, String eoq, boolean num){

		String sql = "SELECT MIN(`"+cn+"`),MAX(`"+cn+"`),";
		sql += "COUNT(`"+cn+"`),COUNT(DISTINCT(`"+cn+"`)) FROM "+eoq;
		return sql;
	}

	public String dataStatsQuery(ArrayList<String> names, String eoq){
		
		String query = "SELECT COUNT(`"+names.get(0)+"`),";
        for(int i = 0; i < names.size(); i++){
            query += "MIN(`"+names.get(i)+"`),MAX(`"+names.get(i)+"`),COUNT(DISTINCT(`"+names.get(i)+"`))";
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
		return tbl_name+".`"+col_name+"`";
	}

	public ResultSet getImportedKeys(DatabaseMetaData dm, String name) throws SQLException {
		return dm.getImportedKeys(null, null, name);
	}

	public String getLimit(){
		return " LIMIT 1";
	}

	public void setQuery(String query){
		this.query = query;
	}

	public Connection getNewConnection(){
		try{
			return DriverManager.getConnection(conn_str);
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

	public String getDistinctQuery(String table, String field){
		String query = "SELECT `"+field+"` FROM ";
		query += "(SELECT * FROM "+table+" ORDER BY `"+field+"` DESC) ";
		query += "GROUP BY `"+field+"`;";
		return query;
	}

	public Connection createTempConnection(String conn_str, String un, String pw) throws SQLException {
		return DriverManager.getConnection(conn_str);
	}

	public Connection getConnection(){
		return conn;
	}

	public String getFormattedName(String name){
		//String[] split = name.split("\\.");
		return name.replace("`","");
	}
}