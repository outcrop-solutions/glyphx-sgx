package synglyphx.data;

import java.io.File;
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
 	private DataFrame data;

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
 		this.tableName = name;
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
 		if(type.equals("sqlite3")){
 			return splitData[splitData.length-1]+":"+tableName;
 		}
 		return splitData[splitData.length-1];
 	}

 	public String getFormattedID(){
 		if(type.equals("sqlite3")){
 			return id+":"+tableName;
 		}
 		return id;
 	}

 	public String getID(){
 		return id;
 	}

}