
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
		String[] sqldbs = JDBCLoader.getInstance().connectToServer(dburl, username, password, db);
		return sqldbs;
	}

	public static String[] chooseDatabase(String db_name){
		String[] tables = JDBCLoader.getInstance().chooseDatabase(db_name);
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
		
		String[] db_list = d.connectToServer("mysql://33.33.33.1","root","jarvis","mysql");
		String[] tables = d.chooseDatabase("world");

		d.setQueryTables("SELECT City.Population, Country.Code FROM City, Country");
		String[] fields = d.getFieldsForTable(0, "mysql");

		for(int i = 0; i < fields.length; i++){
			System.out.println(fields[i]);
		}
		//d.getStatsForField(table, field);
		d.closeConnection();
	}
}