package synglyphx.jdbc;

import java.sql.*;
import synglyphx.io.Logger;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;

public class JDBCLoader {

   private static JDBCLoader instance = null;
   public static Driver driver = null;
   public static Database database = null;
   public static String connectionString;
   public static String username;
   public static String password;
   public static String dbType;
   public static boolean merged;

   protected JDBCLoader(){}

   public static JDBCLoader getInstance() {
      if(instance == null){
         instance = new JDBCLoader();
      }
      return instance;
   }
   
   public static String[] connectToServer(String db_url, String user, String pass, String db) {

      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;

      try{
    
         driver = DriverSelector.getDriver(dbType);
         Class.forName(driver.packageName());

         driver.createConnection(connectionString,username,password);

         database = new Database(driver);

      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }catch(Exception e){
         try{
            e.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }

      return database.getSchemas();
   }

   public static String[] getTableNames(){
      return database.getTableNames();
   }

   public static void setChosenTables(String[] chosen){
      merged = false;
      database.initializeChosenTables(chosen);
   }

   public static void setQueryTables(String query){
      merged = true;
      database.initializeQueryTables(query);
   }

   public static String[] getFieldsForTable(int table){
      if(merged){
         return database.getMergedTable().getColumnNames();
      }
      return database.getTable(table).getColumnNames();
   }

   public static String[] getStatsForField(int table, String field){
      if(merged){
         return database.getMergedTable().getStats(field);
      }
      return database.getTable(table).getStats(field);
   }

   public String[] getForeignKeys(String tableName){
      return database.getForeignKeys(tableName);
   }

   public String[] getSampleData(int table, int row){
      return database.getSampleData(table, row);
   }

   public static void closeConnection(){
      
      try{
         driver.getConnection().close();
         Logger.getInstance().add("");
         Logger.getInstance().add("Closing connection to database...");
      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }

   }

}