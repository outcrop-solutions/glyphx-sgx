package synglyphx.data;

import java.io.*;
import java.sql.*;
import java.util.Map;
import java.util.HashMap;
import java.util.HashSet;
import java.util.ArrayList;
import java.util.List;
import java.util.Collections;
import java.util.LinkedHashSet;
import synglyphx.data.DataStats;
import synglyphx.io.Logger;

public class DataFrame {
	
	private Map<String, Integer> headers = null;
	private ArrayList<ArrayList<String>> data = null;
	private HashMap<String, ArrayList<String>> queryResults = null;
	private boolean first = false;
	private HashMap<String, ArrayList<String>> minMaxTable = null;
	private HashMap<String, Boolean> fieldType = null;
	private HashMap<String, Thread> textInterpolationThreads = null;
	private HashMap<String, ArrayList<String>> textInterpolationFields = null;
	private HashMap<String, Thread> percentRankThreads = null;
	private HashMap<String, HashMap<Double, Double>> percentRankFields = null;
	private String[] headerString;
	//private Thread parse2SQLite;
	private Connection conn;

	public DataFrame(){

	}

	public void addRow(ArrayList<String> csvLine){

		if(headers == null){
			headers = new HashMap<String, Integer>();
			for (int i = 0; i < csvLine.size(); i++) {
				headers.put(csvLine.get(i), i);
			}
			data = new ArrayList<ArrayList<String>>();
		}else{
			data.add(csvLine);
		}
	}

	public ArrayList<String> getColumn(String heading){

		ArrayList<String> temp = new ArrayList<String>();
		for (int i = 0; i < data.size(); i++) {
			temp.add(data.get(i).get(headers.get(heading)));
		}

		return temp;
	}

	public ArrayList<String> getRow(int i){
		return data.get(i);
	}

	public int size(){
		return data.size();
	}

	public void setTextInterpolationFields(ArrayList<String> ti_fields){
		textInterpolationFields = new HashMap<String, ArrayList<String>>();
		textInterpolationThreads = new HashMap<String, Thread>();
		for(int i = 0; i < ti_fields.size(); i++){
			textInterpolationThreads.put(ti_fields.get(i), textInterpolationThread(ti_fields.get(i)));
		}
	}

	public Thread textInterpolationThread(final String field){
		Thread thread = new Thread(){
    		public void run(){
    			ArrayList<String> values = new ArrayList<String>(new HashSet<String>(getColumn(field)));
    			Collections.sort(values, String.CASE_INSENSITIVE_ORDER);
    			textInterpolationFields.put(field, values);
    		}
  		};thread.start();
  		return thread;
	}

	public void setPercentRankFields(ArrayList<String> pr_fields){
		percentRankFields = new HashMap<String, HashMap<Double, Double>>();
		percentRankThreads = new HashMap<String, Thread>();
		System.out.println("PercentRank fields: "+pr_fields.size());
		for(int i = 0; i < pr_fields.size(); i++){
			percentRankThreads.put(pr_fields.get(i), percentRankThread(pr_fields.get(i)));
		}
	}

	public Thread percentRankThread(final String field){
		Thread thread = new Thread(){
    		public void run(){
    			HashMap<Double, Double> pairs = new HashMap<Double, Double>();
    			ArrayList<String> temp = getColumn(field);
    			ArrayList<Double> values = new ArrayList<Double>();
    			for(int i = 0; i < temp.size(); i++){
    				values.add(Double.parseDouble(temp.get(i)));
    			}
    			Collections.sort(values);
    			for(int i = 0; i < values.size(); i++){
					if(!pairs.containsKey(values.get(i))){
						pairs.put(values.get(i), i/(values.size()-1.0));
					}
				}
				percentRankFields.put(field, pairs);
    		}
  		};thread.start();
  		return thread;
	}

	public double getPercentRank(String field, String value){
		try{
			if(percentRankThreads.get(field).getState() != Thread.State.TERMINATED){
				percentRankThreads.get(field).join();
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		return percentRankFields.get(field).get(Double.parseDouble(value));
	}

	public double getTIMax(String field){
		try{
			textInterpolationThreads.get(field).join();
		}catch(InterruptedException ie){
	        ie.printStackTrace();
		}
		return textInterpolationFields.get(field).size()-1;
	}

	public double getTIPos(String field, String value){
		return textInterpolationFields.get(field).indexOf(value);
	}

	public ArrayList<String> getHeaders(){
		
		ArrayList<String> temp = new ArrayList<String>();
		int size = this.data.get(0).size();
		HashMap<Integer,String> ph = new HashMap<Integer,String>();
		for (Map.Entry<String, Integer> entry : this.headers.entrySet()) {
			ph.put(entry.getValue(), entry.getKey());
		}
		headerString = new String[size];
		for(int i=0;i<size;i++){
			temp.add(ph.get(i));
			headerString[i] = ph.get(i);
		}

		return temp;
	}

	public String[] getHeaderString(){
		return headerString;
	}

	public ArrayList<String> distinct(String field){
		return new ArrayList<String>(new HashSet<String>(getColumn(field)));
	}

	public Cursor query(Query q){

		queryResults = new HashMap<String, ArrayList<String>>();

		ArrayList<String[]> queries = q.getQueries();
		ArrayList<Integer> indices = new ArrayList<Integer>();

		if(q.getDistinct()){
			ArrayList<String> dist = new ArrayList<String>();
			ArrayList<String> col = this.getColumn(queries.get(0)[0]);
			for(int i=0; i < col.size(); i++){
				if(!dist.contains(col.get(i))){
					dist.add(col.get(i));
					indices.add(dist.size()-1);
				}
			}
			queryResults.put(queries.get(0)[0], dist);
			Cursor distCursor = new Cursor(queryResults, indices, q);	
			return distCursor;
		}

		for (int i=0; i < queries.size(); i++){
			queryResults.put(queries.get(i)[0], this.getColumn(queries.get(i)[0]));
		}

		if(q.getAllCols()){
			ArrayList<String> col = this.getColumn(queries.get(0)[0]);
			for(int i=0; i<col.size();i++){
				indices.add(i);
			}
			Cursor cursor = new Cursor(queryResults, indices, q);	
			return cursor;
		}

		int count = 0;
		ArrayList<String> temp = null;
		ArrayList<ArrayList<Integer>> all = new ArrayList<ArrayList<Integer>>();
		for (int i=0; i < queries.size(); i++){
			all.add(new ArrayList<Integer>());
			if(queries.get(i).length == 2){
				temp = queryResults.get(queries.get(i)[0]);
				for(int j=0; j<temp.size();j++){
					if(temp.get(j).equals(queries.get(i)[1])){
						all.get(i).add(j);
					}
				}
			}
			else if(queries.get(i).length == 3){
				temp = queryResults.get(queries.get(i)[0]);
				
				for(int j=0; j<temp.size();j++){
					if(queries.get(i)[1].equals("gte")){
						if(Integer.parseInt(temp.get(j)) >= Integer.parseInt(queries.get(i)[2])){
							all.get(i).add(j);
						}						
					}else if(queries.get(i)[1].equals("lte")){
						if(Integer.parseInt(temp.get(j)) <= Integer.parseInt(queries.get(i)[2])){
							all.get(i).add(j);
						}						
					}else if(queries.get(i)[1].equals("gt")){
						if(Integer.parseInt(temp.get(j)) > Integer.parseInt(queries.get(i)[2])){
							all.get(i).add(j);
						}						
					}else if(queries.get(i)[1].equals("lt")){
						if(Integer.parseInt(temp.get(j)) < Integer.parseInt(queries.get(i)[2])){
							all.get(i).add(j);
						}						
					}
				}
			}
		}

		int size = 0;
		int wi = 0;
		for(int i=0; i<all.size();i++){
			if(all.get(i).size() > size){size = all.get(i).size();wi = i;}
		}

		for(int i=0; i < size; i++){
			int t = all.get(wi).get(i);
			boolean c = true;
			for(int j=0; j<all.size();j++){
				if(j != wi && all.get(j).size() != 0){
					if (!all.get(j).contains(t)){c = false;}
				}
			}
			if(c){indices.add(t);}
		}

		Cursor tempCursor = new Cursor(queryResults, indices, q);	

		return tempCursor;
	}

	public boolean isNumeric(int j){
		try  
		{  
		  	for(int i = 0; i < data.size(); i++){
		  		double d = Double.parseDouble(data.get(i).get(j)); 
		  	} 
		}  
		catch(NumberFormatException nfe)  
		{  
		    return false;  
		}  
		return true;  
	}

	public void setFieldTypes(HashMap<String, Boolean> ft){
		this.fieldType = ft;
	}

	public void setMinMaxTable(HashMap<String, ArrayList<String>> mmt){
		this.minMaxTable = mmt;
	}

	public void createMinMaxTable(){
		minMaxTable = new HashMap<String, ArrayList<String>>();
		fieldType = new HashMap<String, Boolean>();

		for (Map.Entry<String, Integer> entry : headers.entrySet()) {
			fieldType.put(entry.getKey(),isNumeric(entry.getValue()));
			ArrayList<String> column = getColumn(entry.getKey());
			//System.out.println(column.size());
			if(fieldType.get(entry.getKey())){
				double min = Double.parseDouble(column.get(0)), max = Double.parseDouble(column.get(0)), avg = 0.0;;
				for(int i=0; i < column.size(); i++){
					double temp = Double.parseDouble(column.get(i));
					if(temp > max){
						max = temp;
					}
					if(temp < min){
						min = temp;
					}
					avg += temp;
					//System.out.println(entry.getKey()+" | "+String.valueOf(min)+" | "+String.valueOf(max)+" | "+String.valueOf(avg/column.size()));
					minMaxTable.put(entry.getKey(),new ArrayList<String>());
					minMaxTable.get(entry.getKey()).add(String.valueOf(min));
					minMaxTable.get(entry.getKey()).add(String.valueOf(max));
					minMaxTable.get(entry.getKey()).add(String.valueOf(avg/column.size()));
				}
			}else{
				String min = column.get(0), max = column.get(0);
				for(int i=0; i < column.size(); i++){
					String temp = column.get(i);
					if(max.compareTo(temp) < 0){
						max = temp;
					}
					if(min.compareTo(temp) > 0){
						min = temp;
					}		
					//System.out.println(entry.getKey()+" | "+min+" | "+max);
					minMaxTable.put(entry.getKey(),new ArrayList<String>());
					minMaxTable.get(entry.getKey()).add(min);
					minMaxTable.get(entry.getKey()).add(max);
				}
			}
		}
		Logger.getInstance().add("Finished creating min/max table...");
	}

	public HashMap<String, DataStats> dataStatsModel(){

		HashMap<String, DataStats> holder = new HashMap<String, DataStats>();
		ArrayList<String> colNames = this.getHeaders();
		Boolean numeric = false;

		for(int i = 0; i < colNames.size(); i++){

			ArrayList<String> rowData = getColumn(colNames.get(i));
			if(fieldType.get(colNames.get(i))){
				numeric = true;
			}

			DataStats temp = null;
			ArrayList<String> dist = new ArrayList<String>(new LinkedHashSet<String>(rowData));

			if(numeric){
				temp = new DataStats(fieldType.get(colNames.get(i)), minMaxTable.get(colNames.get(i)).get(0), minMaxTable.get(colNames.get(i)).get(1), 
					0.0, rowData.size(), dist.size());
			}else{
				Collections.sort(rowData);
				temp = new DataStats(fieldType.get(colNames.get(i)), rowData.get(0), rowData.get(rowData.size()-1), 0.0, rowData.size(), dist.size());
			}

			holder.put(colNames.get(i), temp);
			temp = null;
			numeric = false;
		}
		Logger.getInstance().add("Returning data stats model...");

		return holder;
	}

	public void createSQLiteTable4CSV(String path) throws Exception{
		//parse2SQLite = new Thread(){
    		//public void run(){
    	createTable(path);
    		//}
  		//};
  		//parse2SQLite.start();
	}

	public void wait4CSV2SQLite(){
		/*try{
			parse2SQLite.join();
		}catch(InterruptedException ie){
	        ie.printStackTrace();
		}*/
	}

	public Connection getConn(){
		return conn;
	}

	private void createTable(String path) {

		ArrayList<String> headings = getHeaders();
		HashMap<String, Boolean> fieldTypes = getFieldType();

		try{
			Class.forName("org.sqlite.JDBC");
			String conn_str = "jdbc:sqlite::memory:";
			conn = DriverManager.getConnection(conn_str);
			String query = "CREATE TABLE if NOT EXISTS 'OnlyTable' ("; 
			String insertQuery = "INSERT INTO 'OnlyTable' VALUES (";
			for(int i = 0; i < headings.size(); i++){
				query += "'"+headings.get(i).replace("'","''")+"'";
				if(fieldTypes.get(headings.get(i))){
					query += " REAL,";
				}else{
					query += " TEXT,";
				}
				insertQuery += "?,";
			}

			query = query.substring(0, query.length()-1);
			insertQuery = insertQuery.substring(0, insertQuery.length()-1);
			query += ");";
			insertQuery += ");";
			
			PreparedStatement pstmt = conn.prepareStatement(query);
		    boolean created = pstmt.execute();

		    pstmt = conn.prepareStatement(insertQuery);

			int size = size();
			for(int i = 0; i < size; i++){
				ArrayList<String> row = getRow(i);
				for(int j = 0; j < row.size(); j++){
					if(row.get(j).equals("")){
						pstmt.setString(j+1, Character.toString((char)128));
					}else{
						pstmt.setString(j+1, row.get(j));
					}
				}
				pstmt.addBatch();
				if(i+1 % 100 == 0){
					int[] updates = pstmt.executeBatch();
					conn.commit();
				}
			}
			int[] updates = pstmt.executeBatch();
			pstmt.close();
			//conn.commit();
		}catch(Exception e){/*
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}*/
	        e.printStackTrace();
	    }
	}

	public HashMap<String, ArrayList<String>> getMinMaxTable(){
		return this.minMaxTable;
	}

	public HashMap<String, Boolean> getFieldType(){
		return this.fieldType;
	}

	public int getHeaderPlace(String h){
		return headers.get(h)+1;
	}
}