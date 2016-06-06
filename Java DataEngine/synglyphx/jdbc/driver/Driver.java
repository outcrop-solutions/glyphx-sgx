package synglyphx.jdbc.driver;

import java.sql.*;
import java.util.ArrayList;

public interface Driver {

	public String packageName();
	public void createConnection(String conn_str, String un, String pw) throws SQLException;
	public String dataStatsQuery(String cn, String eoq, boolean num);
	public String dataStatsQuery(ArrayList<String> names, String eoq);
	public String basicField(String tbl_name, String col_name);
	public String mergedField(String tbl_name, String col_name);
	public ResultSet getImportedKeys(DatabaseMetaData dm, String name) throws SQLException;
	public Connection getNewConnection();
	public Connection createTempConnection(String conn_str, String un, String pw) throws SQLException;
	public Connection getConnection();
	public String getFormattedName(String name);
	public void setQuery(String query);
}