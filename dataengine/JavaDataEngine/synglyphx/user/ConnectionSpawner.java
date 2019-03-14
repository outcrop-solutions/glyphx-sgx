package synglyphx.user;

import java.sql.*;
import java.io.*;
import java.util.Date;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.TimeUnit;

import synglyphx.io.Logger;

public class ConnectionSpawner {
	/*
	private static String HOST = "sgxinstance.cqu1pnqua5at.us-west-2.rds.amazonaws.com";
	private static String USER = "sgxadmin";
	private static String PASS = "#sgxpw13#";
	private static String DBNAME = "sgxdb";
	private static String PORT = "3306";
	*/
	public ConnectionSpawner(){}

	public static Connection spawnConnection(){
		try{

			AWSCredentials cred = AWSCredentials.getInstance();
			Class.forName("com.mysql.jdbc.Driver");
			//String conn_str = "jdbc:mysql://"+HOST+":"+PORT+"/"+DBNAME;
			//return DriverManager.getConnection(conn_str, USER, PASS);
			String conn_str = "jdbc:mysql://"+cred.getRDSHost()+":"+cred.getRDSPort()+"/"+cred.getRDSDbname();
			return DriverManager.getConnection(conn_str, cred.getRDSUser(), cred.getRDSPass());

		}catch(Exception e){
			try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
            e.printStackTrace();
			return null;
		}
	}
}