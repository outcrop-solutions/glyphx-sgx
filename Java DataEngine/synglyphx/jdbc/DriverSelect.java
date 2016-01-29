package synglyphx.jdbc;

public class DriverSelect {

	public DriverSelect(){}

	public static String getDriver(String database){

		String driver;

		switch(database){
			case "mysql": driver = "com.mysql.jdbc.Driver";
						break;
			case "sqlite": driver = "org.sqlite.JDBC";
						break;
			case "sqlite3": driver = "org.sqlite.JDBC";
						break;
			case "vertica": driver = "com.vertica.jdbc.Driver";
						break;
			default: driver = "Invalid database";
						break;
		}
		
		return driver;
	}
}