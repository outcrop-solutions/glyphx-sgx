package synglyphx.data;

import java.io.*;
import java.util.Map;
import java.util.HashMap;
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
	private String[] headerString;

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
					Double.parseDouble(minMaxTable.get(colNames.get(i)).get(2)), rowData.size(), dist.size());
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