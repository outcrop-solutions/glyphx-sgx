package synglyphx.data;

import java.io.File;
import java.util.ArrayList;
import java.util.regex.Pattern;
import synglyphx.util.Grid;
import synglyphx.jdbc.driver.Driver;

public class SourceDataInfo {

 	private String id;
 	private String path;
 	private int rootId;
 	private int lastId;
 	private String tableName;
 	private String host;
 	private String username;
 	private String password;
 	private String type;
 	private String query;
 	private DataFrame data = null;
 	//private String base_table;
 	//private String lookup_table;
 	//private String foreign_key;
 	//private String primary_key;
 	private boolean merged_table;
 	private boolean hasTextInterpolation;
 	private boolean hasPercentRank;
 	private ArrayList<String> textInterpolationFields = null;
 	private ArrayList<String> percentRankFields = null;
 	private ArrayList<String> boundFields = null;
 	private Driver tempDriver = null;
 	private Grid grid = null;
 	//private ArrayList<String> inputFields;

 	public SourceDataInfo(){
 		this.hasTextInterpolation = false;
 		this.hasPercentRank = false;
 		this.boundFields = new ArrayList<String>();
 	}

 	public void setHasTextInterpolation(){
 		if(!hasTextInterpolation){
 			textInterpolationFields = new ArrayList<String>();
 		}
 		this.hasTextInterpolation = true;
 	}

 	public void setHasPercentRank(){
 		if(!hasPercentRank){
 			percentRankFields = new ArrayList<String>();
 		}
 		this.hasPercentRank = true;
 	}

 	public void setupGrid(String heading){
 		if(this.grid == null && data != null){
 			this.grid = new Grid(data.getColumn(heading));
 		}
 	}

 	public void setDriver(Driver driver){
 		tempDriver = driver;
 	}

 	public void setRootID(int id){
 		this.rootId = id;
 	}

 	public void setLastID(int id){
 		this.lastId = id;
 	}

 	public void setDataFrame(DataFrame data){
 		this.data = data;
 	}

 	public void setPath(String path){
 		this.path = path;
 	}

 	public void setID(String id){
 		this.id = id;
 	}

 	public void setTable(String name){
 		merged_table = false;
 		this.tableName = name;
 		this.query = "SELECT * FROM "+name;
 	}

 	public void setHost(String host){
 		this.host = host;
 	}

 	public void setUsername(String user){
 		this.username = user;
 	}

 	public void setPassword(String pass){
 		this.password = pass;
 	}
 	
 	public void setType(String type){
 		this.type = type;
 	}

 	public void setQuery(String query){
 		if(tableName.equals("Merged"))
 			merged_table = true;
 		this.query = query;
 	}

 	public void addBoundField(String field_id){
 		boundFields.add(field_id);
 	}

 	public void addTextInterpolationField(String ti_field){
 		textInterpolationFields.add(ti_field);
 	}

 	public void addPercentRankField(String pr_field){
 		percentRankFields.add(pr_field);
 	}
/*
 	public void setInputFields(ArrayList<ArrayList<String>> inputs){
 		inputFields = new ArrayList<String>();
 		for(int i = 0; i < inputs.size(); i++){
 			inputFields.add(inputs.get(i).get(2));
 		}
 	}

 	public void setBaseTableName(String tblname){
 		this.base_table = tblname;
 	}

 	public void parseForeignKeyData(String fkdata){
 		merged_table = true;
 		String[] fk_split = fkdata.split(",");
 		lookup_table = fk_split[1];
 		foreign_key = fk_split[0];
 		primary_key = fk_split[2];
 		createQuery();
 	}
*/
 	public boolean isMerged(){
 		return merged_table;
 	}

 	public boolean hasTextInterpolation(){
 		return hasTextInterpolation;
 	}

 	public boolean hasPercentRank(){
 		return hasPercentRank;
 	}

 	public boolean hasBoundField(String field_id){
 		return boundFields.contains(field_id);
 	}

 	public Driver getDriver(){
 		return tempDriver;
 	}

 	public String getHost(){
 		return host;
 	}

 	public String getUsername(){
 		return username;
 	}

 	public String getPassword(){
 		return password;
 	}

 	public String getType(){
 		return type;
 	}

 	public String getQuery(){
 		return query;
 	}

 	public String getTable(){
 		return tableName;
 	}

 	public int getRootID(){
 		return rootId;
 	}

 	public int getLastID(){
 		return lastId;
 	}

 	public DataFrame getDataFrame(){
 		return data;
 	}

 	public String getPath(){
 		return path;
 	}

 	public int getRowCount(){
 		return data.size();
 	}

 	public void setTextInterpolationFields(){
 		if(data != null){
 			data.setTextInterpolationFields(textInterpolationFields);
 		}
 	}

 	public void setPercentRankFields(){
 		if(data != null){
 			data.setPercentRankFields(percentRankFields);
 		}
 	}

 	public String getFormattedName(){
 		String[] splitData = path.split(Pattern.quote(File.separator));
 		splitData = splitData[splitData.length-1].split("/");
 		
 		if(type.equals("oracle")){
 			return tableName;
 		}else if(!type.equals("csv")){
 			return splitData[splitData.length-1]+":"+tableName;
 		}
 		return splitData[splitData.length-1];
 	}

 	public String getFormattedID(){
 		if(!type.equals("csv")){
 			return id+":"+tableName;
 		}
 		return id;
 	}

 	public String getID(){
 		return id;
 	}

 	public Grid getGrid(){
 		return grid;
 	}
/*
 	public ArrayList<String> getInputFields(){
 		return inputFields;
 	}

 	public String getBaseTableName(){
 		return base_table;
 	}

 	public String getLookupTableName(){
 		return lookup_table;
 	}

 	public String getForeignKey(){
 		return foreign_key;
 	}

 	public String getPrimaryKey(){
 		return primary_key;
 	}

 	private void createQuery(){
		//SELECT City.Population, Country.Code FROM 
		//(City INNER JOIN Country ON (City.CountryCode=Country.Code));
		String q = "SELECT ";
		for(int i = 0; i < inputFields.size(); i++){
			q += inputFields.get(i);
			if(i != inputFields.size()-1){q += ", ";}
		}
		q += " FROM (";
		q += base_table;
		q += " INNER JOIN ";
		q += lookup_table;
		q += " ON (";
		q += foreign_key+"="+primary_key;
		q += "))";

		this.query = q;
	}
*/
}