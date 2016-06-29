package synglyphx.jdbc;

import java.sql.*;
import synglyphx.io.Logger;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;
import synglyphx.util.ErrorHandler;

public class JDBCLoader {

   private static JDBCLoader instance = null;
   public static Driver driver = null;
   public static Database database = null;
   public static String connectionString = null;
   public static String username = null;
   public static String password = null;
   public static String dbType;
   public static boolean merged;
   public static int closeCount = 1;

   protected JDBCLoader(){}

   public static JDBCLoader getInstance() {
      if(instance == null){
         instance = new JDBCLoader();
      }
      return instance;
   }
   
   public static int connectToServer(String db_url, String user, String pass, String db) {

      boolean same_conn = false;
      if(connectionString != null && username != null && password != null){
         if(connectionString.equals("jdbc:"+db_url) && username.equals(user) && password.equals(pass)){
            same_conn = true;
         }
      }
      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;
 
      try{
         if(same_conn && !driver.getConnection().isClosed()){
            return 0;
         }
         Logger.getInstance().add(username +" | "+ password);
         driver = DriverSelector.getDriver(dbType);
         Class.forName(driver.packageName());
         Logger.getInstance().add("Loaded driver for "+dbType);

         driver.createConnection(connectionString,username,password);
         System.out.println("Driver is closed: " + driver.getConnection().isClosed());
         System.out.println("Driver is valid: " + driver.getConnection().isValid(1000));
         Logger.getInstance().add("Connection created");

         double start = 0.0;
         double end = 0.0;
         start = System.currentTimeMillis();
         database = new Database(driver);
         end = System.currentTimeMillis();
         System.out.print("Create database: ");
         System.out.println((end-start)/1000.00);
         Logger.getInstance().add("Database created");

      }catch(Exception e){
         System.out.println("Failed1");
         try{
            e.printStackTrace(ErrorHandler.getInstance().addError());
            return 1;
         }catch(Exception ex){}
         e.printStackTrace();
      }
      return 0;
   }

   public static String[] getSchemas(){
      return database.getSchemas();
   }

   public static String[] getTableNames(){
      return database.getTableNames();
   }

   public static String[] getSchemaTableNames(String sch){
      return database.getSchemaTableNames(sch);
   }

   public static int sizeOfQuery(String query){
      return database.sizeOfQuery(query);
   }

   public static void setChosenTables(String[] chosen){
      merged = false;
      database.initializeChosenTables(chosen);
   }

   public static void setQueryTables(String query){
      merged = true;
      database.initializeQueryTables(query, driver);
   }

   public static void queueATable(String name, String query){
      merged = false;
      database.queueATable(name, query);
   }

   public static void removeQueuedTable(String name){
      database.removeQueuedTable(name);
   }

   public static void executeQueuedTables(){
      database.executeQueuedTables();
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

   public static String[] getForeignKeys(String tableName){
      return database.getForeignKeys(tableName);
   }

   public static String[] getSampleData(int table, int row){
      return database.getSampleData(table, row);
   }

   public static void closeConnection(){
      
      if(closeCount == 2){
         try{
            if(!driver.getConnection().isClosed()){
               driver.getConnection().close();
               Logger.getInstance().add("");
               Logger.getInstance().add("Closing connection to database...");
            }
         }catch(Exception e){
            try{
               e.printStackTrace(ErrorHandler.getInstance().addError());
            }catch(Exception ex){}
            e.printStackTrace();
         }
         closeCount = 0;
      }
      closeCount += 1;

   }

}