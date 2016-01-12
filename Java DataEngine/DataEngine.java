
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

	public static void loadFromSQLite(final String path){
		Logger.getInstance().add("Loading SQLite db...");
		SQLiteReader reader = new SQLiteReader();
		reader.readTables(path);
		tableNames = reader.getTableNames();
		currPath = path;
	}

	public static void setTable(int i){
		SQLiteReader reader = new SQLiteReader();
		reader.createDataFrame(currPath,tableNames.get(i));
		DataFrame data = reader.getDataFrame();
		createDataStats(data);
	}

	public static void createDataStats(DataFrame data){
		Logger.getInstance().add("Creating data stats model...");
		dataStats = data.dataStatsModel();
		headers = data.getHeaders();
		headerString = data.getHeaderString();
	}

	public static int size(){
		return headers.size();
	}

	public static int getTableCount(){
		try{
			return tableNames.size();
		}catch(Exception e){
			return 1;
		}
	}

	public static String getTableName(int i){
		return tableNames.get(i);
	}
/*
	public static String getColumnName(int i){
		return headers.get(i);
	}

	public static String getType(int i){
		String type = dataStats.get(headers.get(i)).getType();
		return type;
	}

	public static String getMin(int i){
		String min = dataStats.get(headers.get(i)).getMin();
		return min;
	}

	public static String getMax(int i){
		String max = dataStats.get(headers.get(i)).getMax();
		return max;
	}

	public static String getAverage(int i){
		String avg = dataStats.get(headers.get(i)).getAverage();
		return avg;
	}

	public static String getCount(int i){
		String count = dataStats.get(headers.get(i)).getCount();
		return count;
	}

	public static String getDistinct(int i){
		String dist = dataStats.get(headers.get(i)).getDistinct();
		return dist;
	}
*/
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

	public static String[] connectToServer(final String dburl, final String username, final String password, final String db){
		String[] sqldbs = JDBCLoader.getInstance().connectToServer(dburl, username, password, db);
		return sqldbs;
	}

	public static String[] chooseDatabase(final String db_name){
		String[] tables = JDBCLoader.getInstance().chooseDatabase(db_name);
		return tables;
	}

	public static void setChosenTables(String[] chosen){
		JDBCLoader.getInstance().setChosenTables(chosen);
	}

	public static void setQueryTables(String query){
		JDBCLoader.getInstance().setQueryTables(query);
	}

	public static String[] getFieldsForTable(int table, String type){
		sourceType = type;
		if(sourceType.equals("csv")){
			return headerString;
		}else if(sourceType.equals("sqlite")){
			setTable(table);
			return headerString;
		}else{
			String[] fields = JDBCLoader.getInstance().getFieldsForTable(table);
			return fields;
		}
	}

	public static String[] getStatsForField(int table, String field){
		String[] stats;
		if(sourceType.equals("csv") || sourceType.equals("sqlite")){
			stats = getStatRow(field);
		}else{
			stats = JDBCLoader.getInstance().getStatsForField(table, field);
		}
		return stats;
	}

//JDBC END

	public static void main(String [] args){

		DataEngine d = new DataEngine();
		
		String[] db_list = d.connectToServer("mysql://33.33.33.1","root","jarvis","mysql");

		String[] tables = d.chooseDatabase("world");

		String[] chosen = new String[2];
		chosen[0] = "City";
		chosen[1] = "Country";

		d.setChosenTables(chosen);

		String[] fields = getFieldsForTable(0, "mysql");
		for (int i = 0; i < fields.length; i++){
			System.out.println(fields[i]);
		}
/*
		d.loadFromSQLite("C:/Users/Bryan/Desktop/test_for_ray/exoplanet_south.db");
		String[] fields = d.getFieldsForTable(0,"sqlite");

		for(int i = 0; i < fields.length; i++){
			System.out.println(fields[i]);
		}

		String[] stats = getStatsForField(0,"rank");

		for(int i = 0; i < stats.length; i++){
			System.out.println(stats[i]);
		}*/
	}
}