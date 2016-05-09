
import java.sql.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Properties;
//import synglyphx.jdbc.DriverSelect;
import synglyphx.jdbc.Database;
import synglyphx.io.Logger;

public class VerticaTest { 

   public static Connection conn = null;
   public static Database database;
   public static String connectionString;
   public static String username;
   public static String password;
   public static String dbType;
   public static ArrayList<String> columnNames;
   public static ArrayList<String> data;
/*
   public static void connectToServer(String db_url, String user, String pass, String db) {

      connectionString = "jdbc:"+db_url;
      username = user;
      password = pass;
      dbType = db;

      //String[] table_names = new String[1]; 

      try{
    
         //Class.forName(DriverSelect.getDriver(dbType));

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
*/
   public static void foreignKeys(String db_url, String user, String pass, String db){

      try{

         Class.forName("com.vertica.jdbc.Driver");
         conn = DriverManager.getConnection("jdbc:"+db_url, user, pass);

         DatabaseMetaData dm = conn.getMetaData();
         ResultSet rs = dm.getImportedKeys(null, null, null);

         while (rs.next()) {
            String colName = rs.getString(8);
            String schtbl = rs.getString(2);
            String tblName = rs.getString(3);
            String orgName = rs.getString(4);
            String fktblName = rs.getString(7);
            System.out.println(colName+" | "+schtbl+" | "+tblName+" | "+orgName+" | "+fktblName);
         }

         rs.close();
       }catch(SQLException se){
         try{
               se.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
       }catch(Exception e){
          e.printStackTrace();
       }
   }

   public static ArrayList<String> getColumnNames(String db_url, String user, String pass, String db){

      ArrayList<String> names = new ArrayList<String>();
      try{

         Class.forName("com.vertica.jdbc.Driver");
         conn = DriverManager.getConnection("jdbc:"+db_url, user, pass);

         String query = "SELECT * FROM (public.inventory_fact "; 
         query += "INNER JOIN public.warehouse_dimension ON (public.inventory_fact.warehouse_key=public.warehouse_dimension.warehouse_key) ";
         query += "INNER JOIN public.product_dimension ON (public.inventory_fact.product_key=public.product_dimension.product_key) ";
         query += "INNER JOIN public.date_dimension ON (public.inventory_fact.date_key=public.date_dimension.date_key))";
         //query += " WHERE inventory_fact.date_key >= 1 AND inventory_fact.date_key <= 913";
         String sql = query+" LIMIT 1";
         PreparedStatement pstmt = conn.prepareStatement(sql);
         ResultSet rs = pstmt.executeQuery();
         ResultSetMetaData metaData = rs.getMetaData();

         int rowCount = metaData.getColumnCount();
         System.out.println(rowCount);
         for (int i = 0; i < rowCount; i++) {
            names.add(metaData.getTableName(i+1)+"."+metaData.getColumnName(i + 1));
         }
         rs.close();

      }catch(SQLException se){
         try{
            se.printStackTrace(Logger.getInstance().addError());
         }catch(Exception ex){}
      }catch(Exception e){
          e.printStackTrace();
       }
      return names;

   }

   public static void columnDataStats(ArrayList<String> names){

      String[] all = new String[3];
      try{

         String query = "SELECT ";

         for(int i = 0; i < names.size(); i++){
            query += "MIN("+names.get(i)+"),MAX("+names.get(i)+"),COUNT(DISTINCT("+names.get(i)+"))";
            if(i != names.size()-1){
               query += ", ";
            }
         }

         query += " FROM (public.inventory_fact "; 
         query += "INNER JOIN public.warehouse_dimension ON (public.inventory_fact.warehouse_key=public.warehouse_dimension.warehouse_key) ";
         query += "INNER JOIN public.product_dimension ON (public.inventory_fact.product_key=public.product_dimension.product_key) ";
         query += "INNER JOIN public.date_dimension ON (public.inventory_fact.date_key=public.date_dimension.date_key))";
         //query += " WHERE inventory_fact.date_key >= 1 AND inventory_fact.date_key <= 913";
         Logger.getInstance().add(query);

         Statement stmt = conn.createStatement();
         ResultSet rs = stmt.executeQuery(query);

         rs.next();

         for(int i = 0; i < names.size()*3; i+=3){
            String min = rs.getString(i+1);
            String max = rs.getString(i+2);
            String distinct = rs.getString(i+3);
            Logger.getInstance().add(min+" | "+max+" | "+distinct); 
         }


            rs.close();
            conn.close();
        }catch(SQLException se){
         try{
               se.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
        }catch(Exception e){
          e.printStackTrace();
       }
   }

   public static void main(String[] args){

      ArrayList<String> names = VerticaTest.getColumnNames("vertica://54.67.93.24:5433/verticanow", "synglyphx_user", "Synglyphx_user@9102", "vertica");
      double start = 0.0;
      double end = 0.0;
      start = System.currentTimeMillis();
      VerticaTest.columnDataStats(names);
      end = System.currentTimeMillis();
      System.out.println((end-start)/1000.00);
/*
      ArrayList<String> names = new ArrayList<String>();
      names.add("online_sales.online_sales_fact");
      names.add("store.store_sales_fact");

      for(int i = 0; i < names.size(); i++){
         VerticaTest.chosenTable(names.get(i));
      }
*/

   }

}