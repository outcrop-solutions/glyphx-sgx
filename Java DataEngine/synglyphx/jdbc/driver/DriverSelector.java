package synglyphx.jdbc.driver;

public class DriverSelector {

	public static Driver getDriver(String type){

		switch(type){
			case "mysql": return new MySQLDriver();
			case "sqlite3": return new SQLiteDriver();
			case "vertica": return new VerticaDriver();
			//case "postgresql": return new PostgreSQLDriver();
			//case "oracle": return new OracleDriver();
			//case "mssqlserver": return new MSSQLServerDriver();
			//case "sparksql": return new SparkSQLDriver();
			default: return null;
		}
	}
}