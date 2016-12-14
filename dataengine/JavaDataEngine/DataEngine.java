
import synglyphx.data.DataFrame;
import synglyphx.data.DataStats;
import synglyphx.io.CSVReader;
//import synglyphx.io.SQLiteReader;
import synglyphx.jdbc.JDBCLoader;
import synglyphx.jdbc.Database;
import synglyphx.io.Logger;
import synglyphx.util.AESencrp;
import synglyphx.util.ErrorHandler;
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
	private static String VERSION = "0.2.05.0";
	
	public static String getVersion(){
		return VERSION;
	}

	public static int loadFromCSV(final String path){
		Logger.getInstance().add("Loading CSV...");
		try{
			CSVReader reader = new CSVReader();
			reader.createDataFrame(path);
			DataFrame data = reader.getDataFrame();
			createDataStats(data);
		}catch(Exception e){
			try{
			    e.printStackTrace(ErrorHandler.getInstance().addError());
			}catch(Exception er){}
		}
		return ErrorHandler.getInstance().hasErrors();
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

	public static String encryptPassword(String password){
		Logger.getInstance().addT(password);
		String encrypted = AESencrp.encrypt(password);
		Logger.getInstance().addT(encrypted);
		return encrypted;
	}

	public static String decryptPassword(String encrypted){
		Logger.getInstance().addT(encrypted);
		String decrypted = AESencrp.decrypt(encrypted);
		Logger.getInstance().addT(decrypted);
		return decrypted;
	}

	public static String[] getErrors(){
		return ErrorHandler.getInstance().getErrors();
	}

	public static void clearErrors(){
		ErrorHandler.getInstance().clearErrors();
	}

//JDBC ACCESSOR METHODS

	public static int connectToServer(String dburl, String username, String password, String db){
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		sourceType = db;
		Logger.getInstance().addT("connectToServer");
		JDBCLoader.getInstance().connectToServer(dburl, username, password, db);
		end = System.currentTimeMillis();
		Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
		return ErrorHandler.getInstance().hasErrors();
	}

	public static String[] getSchemas(){
		return JDBCLoader.getInstance().getSchemas();
	}

	public static int sizeOfQuery(String query){
		Logger.getInstance().addT("sizeOfQuery");
		int size = JDBCLoader.getInstance().sizeOfQuery(query);
		return size;
	}

	public static String[] chooseDatabase(String db){
		Logger.getInstance().addT("chooseDatabase");
		String[] tbls = getTableNames();
		return tbls;
	}

	public static String[] getTableNames(){
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		Logger.getInstance().addT("getTableNames");
		String[] tables = JDBCLoader.getInstance().getTableNames();
		end = System.currentTimeMillis();
		Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
		return tables;
	}

	public static String[] getSchemaTableNames(String sch){
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		Logger.getInstance().addT("getSchemaTableNames");
		String[] tables = JDBCLoader.getInstance().getSchemaTableNames(sch);
		end = System.currentTimeMillis();
		Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
		return tables;
	}

	public static void setChosenTables(String[] chosen){
		Logger.getInstance().addT("setChosenTables");
		JDBCLoader.getInstance().setChosenTables(chosen);
	}

	public static void setQueryTables(String query){
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		Logger.getInstance().addT("setQueryTables");
		Logger.getInstance().add(query);
		JDBCLoader.getInstance().setQueryTables(query);
		end = System.currentTimeMillis();
		Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
	}

	public static void queueATable(String name, String query){
		JDBCLoader.getInstance().queueATable(name, query);
	}

	public static void removeQueuedTable(String name){
		JDBCLoader.getInstance().removeQueuedTable(name);
	}

	public static void executeQueuedTables(){
		JDBCLoader.getInstance().executeQueuedTables();
	}

	public static String[] getFieldsForTable(int table, String db){
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		Logger.getInstance().addT("getFieldsForTable");
		sourceType = db;
		if(sourceType.equals("csv")){
			end = System.currentTimeMillis();
			Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
			return headerString;
		}else{
			String[] fields = JDBCLoader.getInstance().getFieldsForTable(table);
			end = System.currentTimeMillis();
			Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
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
		Logger.getInstance().addT("closeConnection");
		JDBCLoader.getInstance().closeConnection();
	}
//JDBC END
	
	public static void main(String [] args){

		DataEngine d = new DataEngine();
		//String enc = d.encryptPassword("Synglyphx_user@9102");
		//String dec = d.decryptPassword(enc);
		//System.out.println(enc);
		//System.out.println(dec);
		/*
		int err = d.loadFromCSV("C:/Users/Bryan/Desktop/East Coast Only/Data_Sample_Mockup_Trimmed.csv");
		if(err == 1){
			String [] errors = d.getErrors();
			System.out.println("Error List:\n");
			for(int i = 0; i < errors.length; i++){
				System.out.println(errors[i]);
			}
		}else{
			String[] fields = d.getFieldsForTable(0,"csv");
			for(int i = 0; i < fields.length; i++){
				//System.out.println(fields[i]);
				String[] stats = d.getStatsForField(0,fields[i]);
				System.out.println(stats[0]+", "+stats[1]+", "+stats[2]+", "+stats[3]+", "+stats[4]+", "+stats[5]);
			}
		}*/
		int err = d.connectToServer("sqlite:C:/Users/Bryan/Desktop/Link Test Exo/test_exoplanet.db","","","sqlite3");
		String[] tbls = d.getTableNames();
		for(int i = 0; i < tbls.length; i++){
			System.out.println(tbls[i]);
		}
		String[] sch_list = d.getSchemas();
		//String[] sch_list = d.connectToServer("mysql://10.128.132.153:3306/world","synglyphx","password","mysql");
		/*String host = "oracle://@10.128.132.153:1521:orcl";
		String user = "nduser";
		String pw = "ndpw";
		String type = "oracle";
		String[] sch_list = d.connectToServer(host,user,pw,type);
	
		double start = 0.0;
		double end = 0.0;
		start = System.currentTimeMillis();
		String[] sch_list = d.connectToServer("vertica://54.67.93.24:5433/verticanow", "synglyphx_user", "Synglyphx_user@9102", "vertica");
	
		System.out.println("Schema List:");
		for(int i = 0; i < sch_list.length; i++){
			System.out.println(sch_list[i]);
		}
		end = System.currentTimeMillis();
		System.out.print("Connect to server: ");
		System.out.println((end-start)/1000.00);
		*//*
		double start1 = 0.0;
		double end1 = 0.0;
		start1 = System.currentTimeMillis();
		String[] tbl_names = d.getTableNames();
		//System.out.println("Table List:");
		for(int i = 0; i < tbl_names.length; i++){
			System.out.println(tbl_names[i]+" | "+String.valueOf(d.sizeOfQuery("SELECT * FROM "+tbl_names[i])));
		}
		end1 = System.currentTimeMillis();
		System.out.print("Get table names: ");
		System.out.println((end1-start1)/1000.00);

		double start2 = 0.0;
		double end2 = 0.0;
		start2 = System.currentTimeMillis();
		String[] sch_tbls = d.getSchemaTableNames("NDUSER");/*
		end2 = System.currentTimeMillis();
		System.out.print("Get schema table names: ");
		System.out.println((end2-start2)/1000.00);
		System.out.println("");
		for(int i = 0; i < sch_tbls.length; i++){
			System.out.println(sch_tbls[i]);
		}
/*
		d.queueATable("online_sales.call_center_dimension", "SELECT * FROM online_sales.call_center_dimension WHERE call_center_key >= 1 AND call_center_key <= 5");
		d.queueATable("public.inventory_fact", "SELECT * FROM public.inventory_fact WHERE product_key >= 1 AND product_key <= 100");
		d.removeQueuedTable("online_sales.call_center_dimension");
		d.executeQueuedTables();

		for(int j = 0; j < 2; j++){
			String[] fields = d.getFieldsForTable(j, "vertica");
			for(int i = 0; i < fields.length; i++){
				String[] stats = d.getStatsForField(j, fields[i]);
				System.out.println(fields[i]+", "+stats[0]+", "+stats[1]+", "+stats[2]+", "+stats[3]+", "+stats[4]+", "+stats[5]);
			}
			System.out.println("");
		}*/
/*
		double start2 = 0.0;
		double end2 = 0.0;
		start2 = System.currentTimeMillis();
		String[] sch_tbls = d.getSchemaTableNames("online_sales");
		end2 = System.currentTimeMillis();
		System.out.print("Get schema table names: ");
		System.out.println((end2-start2)/1000.00);
		//System.out.println("Schema Table List:");
		
		for(int i = 0; i < sch_tbls.length; i++){
			start = System.currentTimeMillis();
			String[] temp = d.getForeignKeys(sch_tbls[i]);
			end = System.currentTimeMillis();
			System.out.print("Get Fkeys for "+sch_tbls[i]+": ");
			System.out.println((end-start)/1000.00);
		}*/
/*
		String[] stbls = new String[2];
		stbls[0] = "online_sales.call_center_dimension";
		stbls[1] = "online_sales.online_sales_fact";
		
		//String[] stbls = new String[1];
		//stbls[0] = "public.customer_dimension";
		d.setChosenTables(stbls);

		for(int j = 0; j < stbls.length; j++){
			String[] fft = d.getFieldsForTable(j, "vertica");
			for(int i = 0; i < fft.length; i++){
				String[] sts = d.getStatsForField(j,fft[i]);
				System.out.println(fft[i]+" | "+sts[0]+" | "+sts[1]+" | "+sts[2]+" | "+sts[3]+" | "+sts[4]+" | "+sts[5]);
			}
			System.out.println("");
		}
*/
		//String query = "SELECT City.Population, Country.Code "; 
		//query += "FROM (City INNER JOIN Country ON (City.CountryCode=Country.Code))";
		/*String query = "SELECT inventory_fact.qty_in_stock, product_dimension.product_price, ";
		query += "date_dimension.day_of_week, warehouse_dimension.warehouse_name FROM (inventory_fact "; 
		query += "INNER JOIN product_dimension ON (inventory_fact.product_key=product_dimension.product_key) ";
		query += "INNER JOIN date_dimension ON (inventory_fact.date_key=date_dimension.date_key) ";
		query += "INNER JOIN warehouse_dimension ON (inventory_fact.warehouse_key=warehouse_dimension.warehouse_key))";
		String query = "SELECT * FROM store.store_sales_fact"; 
		double t1 = System.currentTimeMillis();
		System.out.println(d.sizeOfQuery(query));
		double t2 = System.currentTimeMillis();
		System.out.println(t2-t1);
		
		String query = "SELECT * FROM (online_sales.online_sales_fact ";
		query += "INNER JOIN public.promotion_dimension ON ";
		query += "(online_sales.online_sales_fact.promotion_key=public.promotion_dimension.promotion_key) ";
		query += "INNER JOIN public.shipping_dimension ON ";
		query += "(online_sales.online_sales_fact.shipping_key=public.shipping_dimension.shipping_key))";

		start = 0.0;
		end = 0.0;
		start = System.currentTimeMillis();
		String query = "SELECT * FROM (public.inventory_fact "; 
		query += "INNER JOIN public.warehouse_dimension ON (public.inventory_fact.warehouse_key=public.warehouse_dimension.warehouse_key) ";
		query += "INNER JOIN public.product_dimension ON (public.inventory_fact.product_key=public.product_dimension.product_key) ";
		query += "INNER JOIN public.date_dimension ON (public.inventory_fact.date_key=public.date_dimension.date_key))";
		//query += " WHERE inventory_fact.date_key >= 1 AND inventory_fact.date_key <= 913";
		
		d.setQueryTables(query);
		String[] fields = d.getFieldsForTable(0, "vertica");

		for(int i = 0; i < fields.length; i++){
			String[] stats = d.getStatsForField(0, fields[i]);
			System.out.println(fields[i]+", "+stats[0]+", "+stats[1]+", "+stats[2]+", "+stats[3]+", "+stats[4]+", "+stats[5]);
		}
		end = System.currentTimeMillis();
		System.out.println((end-start)/1000.00);

		System.out.println(d.sizeOfQuery(query));*/
		d.closeConnection();
		//d.closeConnection();
	}
}
