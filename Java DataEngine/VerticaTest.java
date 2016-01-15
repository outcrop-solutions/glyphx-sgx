
import java.sql.*;
import java.util.Properties;
import synglyphx.jdbc.DriverSelect;
import synglyphx.jdbc.Database;

public class VerticaTest { 

   public static Connection conn = null;
   public static Database database;
   public static String connectionString;
   public static String username;
   public static String password;
   public static String dbType;

   public static String[] connectToServer(String db_url, String user, String pass, String db) {

      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;

      String[] table_names = new String[1]; 

      try{
    
         Class.forName(DriverSelect.getDriver(dbType));

         System.out.println("Connecting...");
         System.out.println(connectionString);
         System.out.println(username);
         System.out.println(password);
         if(dbType.equals("sqlite")){
            conn = DriverManager.getConnection(connectionString);
         }else{
            Properties myProp = new Properties();
            myProp.put("user", username);
            myProp.put("password", password);
            myProp.put("loginTimeout", "35");
            conn = DriverManager.getConnection(connectionString,myProp);
         }

         System.out.println("Connected successfully...");
         database = new Database(conn);
         table_names = database.getTableNames();

      }catch(SQLException se){
         se.printStackTrace();
         try{
            //se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }catch(Exception e){
         e.printStackTrace();
         try{
           //e.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }

      return table_names;
   }

   public static void main(String[] args){

      String[] db_names = VerticaTest.connectToServer("vertica://54.67.93.24:5433/verticanow", "synglyphx_user", "Synglyphx_user@9102", "vertica");

      for(int i = 0; i < db_names.length; i++){
         System.out.println(db_names[i]);
      }


   }

}