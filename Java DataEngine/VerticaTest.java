
import java.sql.*;
import java.io.*;
import java.util.ArrayList;
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
   public static ArrayList<String> columnNames;
   public static ArrayList<String> data;

   public static void connectToServer(String db_url, String user, String pass, String db) {

      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;

      //String[] table_names = new String[1]; 

      try{
    
         Class.forName(DriverSelect.getDriver(dbType));

         System.out.println("Connecting...");
         System.out.println(connectionString);
         System.out.println(username);
         System.out.println(password);

         conn = DriverManager.getConnection(connectionString, username, password);

         System.out.println("Connected successfully...");
        // database = new Database(conn);
        // table_names = database.getTableNames();

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

      //return table_names;
   }

   public static void chosenTable(String tableName){
      getFieldNames(tableName);
      getDataFromTable(tableName);
   }

   public static void getFieldNames(String tableName){
      columnNames = new ArrayList<String>();
      try{

         String sql = "SELECT * FROM "+tableName+" LIMIT 1";
         PreparedStatement pstmt = conn.prepareStatement(sql);
         ResultSet rs = pstmt.executeQuery();
         ResultSetMetaData metaData = rs.getMetaData();

         int rowCount = metaData.getColumnCount();

         for (int i = 0; i < rowCount; i++) {
            String column_name = metaData.getColumnName(i + 1);
            columnNames.add(column_name);
         }

         rs.close();

      }catch(SQLException se){
         se.printStackTrace();
      }
   }

   public static void getDataFromTable(String tableName){
      String outDir = "C:\\Users\\Bryan\\SharePoint\\Synglyphx Team Site - Documents\\Vertica Data Sets\\";
      try{

         String sql = "SELECT * FROM "+tableName;

         Statement stmt = conn.createStatement();
         ResultSet rs = stmt.executeQuery(sql);

         try{
            FileWriter file = new FileWriter(outDir+tableName+".csv");
            BufferedWriter bf = new BufferedWriter(file);

            String names = "";
            for(int i = 0; i < columnNames.size(); i++){
               names += columnNames.get(i);
               if(i != columnNames.size()-1){
                  names += ",";
               }
            }
            names += "\n";
            bf.write(names);

            while(rs.next()){
               String row = "";
               for(int i = 0; i < columnNames.size(); i++){
                  row += rs.getString(columnNames.get(i));
                  if(i != columnNames.size()-1){
                     row += ",";
                  }
               }
               row += "\n";
               bf.write(row);
            }

            bf.close();
         }catch(IOException ioe){
            ioe.printStackTrace();
         }

         rs.close();
      }catch(SQLException se){
         se.printStackTrace();
      }
   }

   public static void writeToCSV(BufferedWriter bf, ResultSet rs) throws IOException, SQLException {

      String names = "";
      for(int i = 0; i < columnNames.size(); i++){
         names += columnNames.get(i);
         if(i != columnNames.size()-1){
            names += ",";
         }
      }
      names += "\n";
      bf.write(names);

      while(rs.next()){
         String row = "";
         for(int i = 0; i < columnNames.size(); i++){
            row += rs.getString(columnNames.get(i));
            if(i != columnNames.size()-1){
               row += ",";
            }
         }
         row += "\n";
         bf.write(row);
      }
   }

   public static void main(String[] args){

      VerticaTest.connectToServer("vertica://54.67.93.24:5433/verticanow", "synglyphx_user", "Synglyphx_user@9102", "vertica");

      ArrayList<String> names = new ArrayList<String>();
      names.add("online_sales.online_sales_fact");
      names.add("store.store_sales_fact");

      for(int i = 0; i < names.size(); i++){
         VerticaTest.chosenTable(names.get(i));
      }


   }

}