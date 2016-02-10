
import synglyphx.data.DataFrame;
import synglyphx.data.DataStats;
import synglyphx.io.CSVReader;
import synglyphx.io.SQLiteReader;
import synglyphx.jdbc.JDBCLoader;
import synglyphx.jdbc.Database;
import synglyphx.io.Logger;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

public class DataEngine {

	private static HashMap<String, DataStats> dataStats = null;
	private static ArrayList<String> headers = null;
	private static String[] headerString;
	private static ArrayList<String> tableNames = null;
	private static String currPath;
	private static String sourceType;
	
	public static void loadFromCSV(final String path){
		Logger.getInstance().add("Loading CSV...");
		CSVReader reader = new CSVReader();
		reader.createDataFrame(path);
		DataFrame data = reader.getDataFrame();
		createDataStats(data);
	}

	public static void createDataStats(DataFrame data){
		Logger.getInstance().add("Creating data stats model...");
		dataStats = data.dataStatsModel();
		headers = data.getHeaders();
		headerString = data.getHeaderString();
	}

	public static String[] getStatRow(String field){
		String[] stats = new String[6];
		stats[0] = dataStats.get(field).getType();
		stats[1] = dataStats.get(field).getMin();
		stats[2] = dataStats.get(field).getMax();
		stats[3] = dataStats.get(field).getAverage();
		stats[4] = dataStats.get(field).getCount();
		stats[5] = dataStats.get(field).getDistinct();
		return stats;
	}

//JDBC ACCESSOR METHODS

	public static String[] connectToServer(String dburl, String username, String password, String db){
		sourceType = db;
		String[] schemas = JDBCLoader.getInstance().connectToServer(dburl, username, password, db);
		return schemas;
	}

	public static String[] getTableNames(){
		String[] tables = JDBCLoader.getInstance().getTableNames();
		return tables;
	}

	public static void setChosenTables(String[] chosen){
		JDBCLoader.getInstance().setChosenTables(chosen);
	}

	public static void setQueryTables(String query){
		JDBCLoader.getInstance().setQueryTables(query);
	}

	public static String[] getFieldsForTable(int table, String db){
		sourceType = db;
		if(sourceType.equals("csv")){
			return headerString;
		}else{
			String[] fields = JDBCLoader.getInstance().getFieldsForTable(table);
			return fields;
		}
	}

	public static String[] getStatsForField(int table, String field){
		String[] stats;
		if(sourceType.equals("csv")){
			stats = getStatRow(field);
		}else{
			stats = JDBCLoader.getInstance().getStatsForField(table, field);
		}
		return stats;
	}

	public static String[] getForeignKeys(String table){
		String[] fkeys = JDBCLoader.getInstance().getForeignKeys(table);
		return fkeys;
	}

	public static String[] getSampleData(int table, int row){
		String[] sample;
		if(sourceType.equals("csv")){
			sample = new String[1];
		}else{
			sample = JDBCLoader.getInstance().getSampleData(table, row);
		}
		return sample;
	}

	public static void closeConnection(){
		JDBCLoader.getInstance().closeConnection();
	}

//JDBC END

	public static void main(String [] args){

		DataEngine d = new DataEngine();
		
		//String[] sch_list = d.connectToServer("mysql://33.33.33.1/world","root","jarvis","mysql");
		//String[] sch_list = d.connectToServer("sqlite://C:/Users/Bryan/Documents/GitHub/DataEngine/Java DataEngine/sqlite_test.db","","","sqlite3");
		String[] sch_list = d.connectToServer("vertica://54.67.93.24:5433/verticanow", "synglyphx_user", "Synglyphx_user@9102", "vertica");

		System.out.println("Schema List:");
		for(int i = 0; i < sch_list.length; i++){
			System.out.println(sch_list[i]);
		}

		String[] tbl_names = d.getTableNames();
		System.out.println("Table List:");
		for(int i = 0; i < tbl_names.length; i++){
			System.out.println(tbl_names[i]);
		}
		
		//String query = "SELECT City.Population, Country.Code "; 
		//query += "FROM (City INNER JOIN Country ON (City.CountryCode=Country.Code))";
		String query = "SELECT inventory_fact.qty_in_stock, product_dimension.product_price, ";
		query += "date_dimension.day_of_week, warehouse_dimension.warehouse_name FROM (inventory_fact "; 
		query += "INNER JOIN product_dimension ON (inventory_fact.product_key=product_dimension.product_key) ";
		query += "INNER JOIN date_dimension ON (inventory_fact.date_key=date_dimension.date_key) ";
		query += "INNER JOIN warehouse_dimension ON (inventory_fact.warehouse_key=warehouse_dimension.warehouse_key))";

		//d.setQueryTables(query);
		d.setQueryTables(query);
		String[] fields = d.getFieldsForTable(0, "vertica");

		for(int i = 0; i < fields.length; i++){
			String[] stats = d.getStatsForField(0, fields[i]);
			System.out.println(fields[i]+", "+stats[0]+", "+stats[1]+", "+stats[2]+", "+stats[3]+", "+stats[4]+", "+stats[5]);
		}
		d.closeConnection();
	}
}
//SELECT inventory_fact.qty_in_stock, product_dimension.product_price, date_dimension.day_of_week, warehouse_dimension.warehouse_name FROM (inventory_fact INNER JOIN product_dimension ON (inventory_fact.product_key=product_dimension.product_key) INNER JOIN date_dimension ON (inventory_fact.date_key=date_dimension.date_key) INNER JOIN warehouse_dimension ON (inventory_fact.warehouse_key=warehouse_dimension.warehouse_key))