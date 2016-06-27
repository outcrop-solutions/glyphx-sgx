package synglyphx.data;

import java.sql.*;
import java.util.ArrayList;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;
import synglyphx.io.Logger;
import synglyphx.util.Functions;

public class FilterField extends BoundField {
	
	public FilterField(String id, String table, String field){
		super(id, table, field);
	}

	public String[] distinctValues(Driver driver){

		ArrayList<String> distinct = new ArrayList<String>();
		try{
	
			String query = driver.getDistinctQuery(table, field);
			PreparedStatement pstmt = driver.getConnection().prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        while(rs.next()){
	        	String temp = rs.getString(field);
	        	if(temp == null){temp = "";}
	        	distinct.add(temp); 	     	
	        }
		    rs.close();

		}catch(SQLException se){
	        try{
	            se.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }catch(Exception e){
	        try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }

	    return Functions.arrayListToStringList(distinct);
	}
}