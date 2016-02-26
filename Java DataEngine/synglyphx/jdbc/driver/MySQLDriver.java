package synglyphx.jdbc.driver;

import java.sql.*;

public class MySQLDriver implements Driver {

	private Connection conn = null;

	public String packageName(){
		return "com.mysql.jdbc.Driver";
	}

	public void createConnection(String conn_str, String un, String pw) throws SQLException {
		this.conn = DriverManager.getConnection(conn_str,un,pw);
	}

	public String dataStatsQuery(String cn, String eoq, boolean num){

		String sql = "SELECT MIN("+cn+"),MAX("+cn+"),AVG("+cn+"),";
		sql += "COUNT("+cn+"),COUNT(DISTINCT("+cn+")) FROM "+eoq;
		return sql;
	}

	public String basicField(String col_name){
		//return "`"+col_name+"`";
		return col_name;
	}

	public String mergedField(String tbl_name, String col_name){
		//return tbl_name+".`"+col_name+"`";
		return tbl_name+"."+col_name;
	}

	public Connection getConnection(){
		return conn;
	}
}