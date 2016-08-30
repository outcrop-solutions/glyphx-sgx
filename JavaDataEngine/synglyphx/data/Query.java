package synglyphx.data;

import java.io.*;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;

public class Query {

	private boolean isDistinct = false;
	private String grouping = "";
	private String[] ordering = null;
	private boolean allCols = false;

	private DataFrame data = null;
	private ArrayList<String[]> incQueries = null;

	public Query(DataFrame data){
		this.data = data;
		incQueries = new ArrayList<String[]>();
	}

	public Query append(String heading){
		String[] tmp = new String[1];
		tmp[0] = heading;
		incQueries.add(tmp);
		return this;
	}

	public Query append(String heading, String value){
		String temp = heading + "," + value;
		String[] splitData = temp.split(",");
		incQueries.add(splitData);
		return this;
	}

	public Query append(String heading, Integer value){
		String temp = heading + "," + value.toString();
		String[] splitData = temp.split(",");
		incQueries.add(splitData);
		return this;
	}

	public Query append(String heading, String operation, Integer value){
		String temp = heading + "," + operation + "," + value.toString();
		String[] splitData = temp.split(",");
		incQueries.add(splitData);
		return this;

	}

	public Query orderBy(String heading){
		String temp = heading + "," + "a";
		ordering = temp.split(",");
		return this;
	}

	public Query orderBy(String heading, String direction){
		String temp = heading + "," + direction;
		ordering = temp.split(",");
		return this;
	}

	public Query groupBy(String heading){
		this.grouping = heading;
		incQueries.add(heading.split(","));
		return this;
	}

	public Query distinct(){
		this.isDistinct = true;
		return this;
	}

	public Query all(){
		ArrayList<String> headers = this.data.getHeaders();
		ArrayList<String> currHead = new ArrayList<String>();
		for(int i=0; i<this.incQueries.size();i++){
			currHead.add(incQueries.get(i)[0]);
		}
		for(int i=0; i<headers.size();i++){
			if(!currHead.contains(headers.get(i))){
				this.append(headers.get(i));
			}
		}
		this.allCols = true;
		//System.out.println(headers.size());
		return this;
	}

	public String[] getOrdering(){
		return this.ordering;
	}

	public String getGrouping(){
		return this.grouping;
	}

	public boolean getDistinct(){
		return this.isDistinct;
	}

	public boolean getAllCols(){
		return this.allCols;
	}

	public ArrayList<String[]> getQueries(){
		//System.out.println(incQueries.size());
		return incQueries;
	}

}
