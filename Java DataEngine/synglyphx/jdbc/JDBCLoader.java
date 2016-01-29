package synglyphx.jdbc;

import java.sql.*;
import synglyphx.io.Logger;

public class JDBCLoader {

   private static JDBCLoader instance = null;
   public static Connection conn = null;
   public static Database database = null;
   public static String connectionString;
   public static String username;
   public static String password;
   public static String dbType;

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

      String[] sqldbs = new String[1];


      try{

         Class.forName(DriverSelect.getDriver(db));

         Logger.getInstance().add("Connecting to Server...");

         if(db.equals("sqlite3")){
            sqldbs[0] = "n/a";
            return sqldbs;
         }else{
            conn = DriverManager.getConnection(connectionString,username,password);
         }

         ResultSet rs;
         if(db.equals("mysql")){
            rs = conn.getMetaData().getCatalogs();
         }else{
            rs = conn.getMetaData().getSchemas();
         }

         int dbcount = 0;

         if(rs.last()){
            dbcount = rs.getRow();
            rs.beforeFirst(); 
         }
         sqldbs = new String[dbcount];
         dbcount = 0;
         while(rs.next()){
            sqldbs[dbcount++] = rs.getString("TABLE_CAT");
         }
         rs.close();
         conn.close();
      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }catch(Exception e){
         try{
            e.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }

      return sqldbs;
   }

   public static String[] chooseDatabase(String db_name){

      String[] table_names = new String[1]; 

      try{
    
         Class.forName(DriverSelect.getDriver(dbType));

         if(dbType.equals("sqlite3")){
            conn = DriverManager.getConnection(connectionString);
         }else{
            conn = DriverManager.getConnection(connectionString+"/"+db_name,username,password);
         }

         database = new Database(conn);
         table_names = database.getTableNames();

      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }catch(Exception e){
         try{
            e.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }

      return table_names;
   }

   public static void setChosenTables(String[] chosen){
      database.initializeChosenTables(chosen);
   }

   public static void setQueryTables(String query){
      database.initializeQueryTables(query);
   }

   public static String[] getFieldsForTable(int table){
      return database.getTable(table).getColumnNames();
   }

   public static String[] getStatsForField(int table, String field){
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
         conn.close();
      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }
      Logger.getInstance().add("");
      Logger.getInstance().add("Closing connection to database...");

   }

}