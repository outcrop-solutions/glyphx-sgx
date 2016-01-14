
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

	public static String[] getFieldsForTable(int table, String type){
		sourceType = type;
		System.out.println(sourceType);
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

//JDBC END

	public static void main(String [] args){

		DataEngine d = new DataEngine();
		
		//String[] db_list = d.connectToServer("mysql://33.33.33.1","root","jarvis","mysql");
		String[] db_list = d.connectToServer("sqlite:C:\\Users\\Bryan\\Desktop\\test_for_ray_1\\test_exoplanet_url.db","","","sqlite");
		String[] tables = d.chooseDatabase("");
		d.setChosenTables(tables);
		String[] names = d.getFieldsForTable(0, "mysql");

		for(int i = 0; i < names.length; i++){
			String[] stats = d.getStatsForField(0, names[i]);
			String toPrint = "";
			toPrint += names[i]+" | ";
			for(int j = 0; j < stats.length; j++){
				toPrint += stats[j]+" | ";
			}
			System.out.println(toPrint);
		}
/*
		String[] chosen = new String[2];
		chosen[0] = "FirstPortion";
		chosen[1] = "SecondPortion";

		d.setChosenTables(chosen);

		String[] fields0 = getFieldsForTable(0, "sqlite");
		String[] fields1 = getFieldsForTable(1, "sqlite");
		
		for(int i = 0; i < fields0.length; i++){
			System.out.println(fields0[i]);
		}

		String[] stats = getStatsForField(0,"rank");

		for(int i = 0; i < stats.length; i++){
			System.out.println(stats[i]);
		}*/
	}
}