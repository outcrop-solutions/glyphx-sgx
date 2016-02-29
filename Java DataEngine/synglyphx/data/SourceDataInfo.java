package synglyphx.data;

import java.io.File;
import java.util.ArrayList;
import java.util.regex.Pattern;

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
 	private DataFrame data;
 	//private String base_table;
 	//private String lookup_table;
 	//private String foreign_key;
 	//private String primary_key;
 	private boolean merged_table;
 	//private ArrayList<String> inputFields;

 	public SourceDataInfo(){

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
 		merged_table = true;
 		this.query = query;
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

 	public String getFormattedName(){
 		String[] splitData = path.split(Pattern.quote(File.separator));
 		splitData = splitData[splitData.length-1].split("/");
 		if(!type.equals("csv")){
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