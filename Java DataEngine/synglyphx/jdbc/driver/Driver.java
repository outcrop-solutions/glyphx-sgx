package synglyphx.jdbc.driver;

import java.sql.*;

public interface Driver {

	public String packageName();
	public void createConnection(String conn_str, String un, String pw) throws SQLException;
	public String dataStatsQuery(String cn, String eoq, boolean num);
	public String basicField(String col_name);
	public String mergedField(String tbl_name, String col_name);
	public Connection getConnection();
}