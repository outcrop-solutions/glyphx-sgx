package synglyphx.jdbc;

import java.sql.*;
import synglyphx.io.Logger;

public class JDBCLoader {

   private static JDBCLoader instance = null;
   public static Connection conn = null;
   public Database database = null;
   public String connectionString;
   public String username;
   public String password;
   public String dbType;

   protected JDBCLoader(){}

   public static JDBCLoader getInstance() {
      if(instance == null){
         instance = new JDBCLoader();
      }
      return instance;
   }
   
   public String[] connectToServer(String db_url, String user, String pass, String db) {

      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;
      String[] sqldbs = new String[1];

      try{

         Class.forName(DriverSelect.getDriver(db));

         Logger.getInstance().add("Connecting to Server...");

         if(db.equals("sqlite")){
            conn = DriverManager.getConnection(connectionString);
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
         se.printStackTrace();
      }catch(Exception e){
         e.printStackTrace();
      }

      return sqldbs;
   }

   public String[] chooseDatabase(String db_name){

      String[] table_names = new String[1]; 

      try{
    
         Class.forName(DriverSelect.getDriver(dbType));

         conn = DriverManager.getConnection(connectionString+"/"+db_name,username,password);

         database = new Database(conn);
         table_names = database.getTableNames();

      }catch(SQLException se){
         se.printStackTrace();
      }catch(Exception e){
         e.printStackTrace();
      }

      return table_names;
   }

   public void setChosenTables(String[] chosen){
      database.initializeChosenTables(chosen);
      closeConnection();
   }

   public void setQueryTables(String query){
      database.initializeQueryTables(query);
      closeConnection();
   }

   public String[] getFieldsForTable(int table){
      return database.getTable(table).getColumnNames();
   }

   public String[] getStatsForField(int table, String field){
      return database.getTable(table).getStats(field);
   }

   public void closeConnection(){
      
      try{
         conn.close();
      }catch(SQLException se){
         se.printStackTrace();
      }
      Logger.getInstance().add("");
      Logger.getInstance().add("Closing connection to database...");
   }

}