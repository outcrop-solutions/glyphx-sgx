package synglyphx.user;

import java.sql.*;
import java.io.*;
import java.util.Date;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.TimeUnit;

public class ConnectionSpawner {

	private static String HOST = "sgxinstance.cqu1pnqua5at.us-west-2.rds.amazonaws.com";
	private static String USER = "sgxadmin";
	private static String PASS = "#sgxpw13#";
	private static String DBNAME = "sgxdb";
	private static String PORT = "3306";

	public ConnectionSpawner(){}

	public static Connection spawnConnection(){
		try{
			Class.forName("com.mysql.jdbc.Driver");
			String conn_str = "jdbc:mysql://"+HOST+":"+PORT+"/"+DBNAME;
			return DriverManager.getConnection(conn_str, USER, PASS);
		}catch(Exception e){
			e.printStackTrace();
			return null;
		}
	}
}