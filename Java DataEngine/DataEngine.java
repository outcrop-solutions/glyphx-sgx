
import synglyphx.data.DataFrame;
import synglyphx.data.DataStats;
import synglyphx.io.CSVReader;
import synglyphx.io.SQLiteReader;
import synglyphx.io.Logger;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;

public class DataEngine {

	private static HashMap<String, DataStats> dataStats = null;
	private static ArrayList<String> headers = null;
	private static ArrayList<String> tableNames = null;
	
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
/*
	public static void main(String [] args){
		DataEngine d = new DataEngine();
		d.loadFromCSV("C:\\Users\\Bryan\\Desktop\\test.csv");
		//d.loadFromSQLite("C:\\Users\\Bryan\\Documents\\GitHub\\DataEngine\\temp.sqlite");
		int t = d.getTableCount();

		for(int i = 0; i < t; i++){
			//d.setTable(i);
			int s = d.size();
			for(int j = 0; j < s; j++){
				System.out.println(d.getColumnName(j)+" | "+d.getType(j)+" | "+d.getMin(j)+" | "+d.getMax(j)+" | "+d.getAverage(j)+" | "+d.getCount(j)+" | "+d.getDistinct(j));
			}
		}
	}*/
}