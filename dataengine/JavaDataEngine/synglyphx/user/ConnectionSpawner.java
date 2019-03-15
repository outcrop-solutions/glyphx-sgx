package synglyphx.user;

import java.sql.*;
import java.io.*;
import java.util.Date;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.TimeUnit;

import synglyphx.io.Logger;

public class ConnectionSpawner {

	private static ConnectionSpawner theInstance = null;
	private static Connection conn = null;

	private ConnectionSpawner(){}

	public static ConnectionSpawner instance() {
  		if (theInstance == null) {
			theInstance = new ConnectionSpawner();
	   	}
	    return theInstance;
  	}

	public static Connection spawnConnection(){

		try{

			if(conn != null && conn.isValid(0)){
				return conn;
			}
			else{
				AWSCredentials cred = AWSCredentials.getInstance();
				Class.forName("com.mysql.jdbc.Driver");
				String conn_str = "jdbc:mysql://"+cred.getRDSHost()+":"+cred.getRDSPort()+"/"+cred.getRDSDbname();
				return DriverManager.getConnection(conn_str, cred.getRDSUser(), cred.getRDSPass());
			}

		}catch(Exception e){
			try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
            e.printStackTrace();
		}
		return null;
	}
}