import java.sql.*;
import com.almworks.sqlite4java.*;
import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;

public class SQLite2Oracle {

	public static HashMap<Integer,String> affinities;
	
	public static void readSQLite(String path, String name){
		affinities = new HashMap<Integer,String>();
		affinities.put(1,"INTEGER");
		affinities.put(2,"REAL");
		affinities.put(3,"TEXT");
		ArrayList<String> temp = null;
		ArrayList<String> type = null;
		SQLiteStatement st0 = null;
		try{
			SQLiteConnection db = new SQLiteConnection(new File(path));
			db.open(true);
		    st0 = db.prepare("SELECT * FROM '"+name+"';"); 
			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 
			temp = new ArrayList<String>();
			type = new ArrayList<String>();
			int size = st0.columnCount();
			for(int i = 0; i<size; i++){
				String str = "";
				str += st0.getColumnName(i);
				temp.add(str);
				//System.out.println(st0.columnType(i));
				type.add(affinities.get(st0.columnType(i)));
			}
			createOracleTable(name, temp, type);

			ArrayList<ArrayList<String>> rows = new ArrayList<ArrayList<String>>();
			int place = 0;
			while(st0.hasRow()){
				rows.add(new ArrayList<String>());
				for(int i = 0; i<size; i++){
					String str = "";
					str += st0.columnValue(i);
					rows.get(place).add(str);
				}
				place += 1;
				st0.step();
			}
			addRowsToOracleTable(name, rows, temp, type);

			db.exec("COMMIT TRANSACTION;");
			//db.close();
		}catch(SQLiteException se){
			se.printStackTrace();
		}
	}//grant select, insert on NDUSER.GLOBALADMISSIONS to nduser;

	public static void createOracleTable(String tblname, ArrayList<String> fields, ArrayList<String> types){
		tblname = tblname.toUpperCase();
		try {
			Class.forName("oracle.jdbc.driver.OracleDriver");
			String url = "jdbc:oracle:thin:@10.128.132.153:1521:orcl";
			//String url = "jdbc:oracle:thin:@localhost:1521/xe";
    		String username = "nduser";
    		String password = "ndpw";

    		String sql = "CREATE TABLE "+tblname+"(";//id NUMBER(11), title VARCHAR2(64))";
    		for(int i = 0; i < fields.size(); i++){
    			sql += fields.get(i);
    			if(fields.get(i).equals("Academic_Rating") || fields.get(i).equals("Non_Academic_Rating") || fields.get(i).equals("OneTest")){
    				types.set(i,"REAL");
    			}
    			if(types.get(i).equals("TEXT")){
    				sql += " VARCHAR2(128) NOT NULL";
    			}else{
    				//sql += " NUMBER(17,6)";
    				sql += " NUMBER(11,2) NOT NULL";
    			}

    			if(i < fields.size()-1){
    				sql += ", ";
    			}else{
    				sql += ")";
    			}
    		}
    		Connection connection = DriverManager.getConnection(url, username, password);
    		DatabaseMetaData dbm = connection.getMetaData();
    		ResultSet tbls = dbm.getTables(null,null,tblname.toUpperCase(),null);
    		Statement statement = connection.createStatement();
    		if(tbls.next()){
    			statement.execute("DROP TABLE "+tblname.toUpperCase());
    		}
    		System.out.println(sql);
    		statement.execute(sql);
    		//statement.execute("GRANT SELECT ON "+tblname.toUpperCase()+" TO PUBLIC");
    		connection.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void addRowsToOracleTable(String tblname, ArrayList<ArrayList<String>> rows, ArrayList<String> fields, ArrayList<String> types){
		int a = 0, b = 0;
		String cp = "";
		tblname = tblname.toUpperCase();
		System.out.println("Populating "+tblname);
		try {
			Class.forName("oracle.jdbc.driver.OracleDriver");
			String url = "jdbc:oracle:thin:@10.128.132.153:1521:orcl";
			//String url = "jdbc:oracle:thin:@localhost:1521/xe";
    		String username = "nduser";
    		String password = "ndpw";

    		Connection connection = DriverManager.getConnection(url, username, password);
    		System.out.println("Connected to server...");
    		Statement statement = connection.createStatement();

    		String sql = "INSERT INTO "+tblname;/*+"( ";
    		for(int i = 0; i <fields.size(); i++){
    			sql += fields.get(i);
    			if(i < fields.size()-1){
    				sql += ", ";
    			}else{
    				sql += ")";
    			}
    		}*/
    		sql += " VALUES (";
    		int count = 0;
    		for(int i = 0; i < rows.size(); i++){
    			ArrayList<String> temp = rows.get(i);
    			cp = sql;
    			for(int j = 0; j < temp.size(); j++){
    				if(types.get(j).equals("INTEGER") || types.get(j).equals("REAL")){
    					cp += temp.get(j);
    				}else{
    					String[] str = temp.get(j).split("'");
    					if(str.length > 1){
    						cp += "'"+str[0]+"''"+str[1]+"'";
    					}else{
    						cp += "'"+temp.get(j)+"'";
    					}
    				} 
    				if(j < temp.size()-1){
    					cp += ", ";
    				}else{
    					cp += ")";
    				}
    				b++;
    			}
    			a++;
   				b = 0;
   				//System.out.println(cp);
    			statement.executeUpdate(cp);
    			cp = "";
    			if(i % (rows.size()/100) == 0){
    				System.out.print(count++);
    				System.out.print("...");
    			}
    		}

    		connection.close();
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println(a);
			System.out.println(b);
			System.out.println(cp);
		}
	}

	public static void revokeAllForSchema(String sch){
		try{
			Class.forName("oracle.jdbc.driver.OracleDriver");
			String url = "jdbc:oracle:thin:@10.128.132.153:1521:orcl";
    		String username = "sys as sysdba";
    		String password = "syspw";

    		Connection connection = DriverManager.getConnection(url, username, password);
    		Statement stmt = connection.createStatement();
    		//DatabaseMetaData md = connection.getMetaData();
	   
        	//ResultSet priv = md.getTablePrivileges(null,sch,null);
        	ResultSet rs = stmt.executeQuery("select object_name from user_objects where object_type = 'TABLE'");
        	while(rs.next()){
	        	String tb_name = rs.getString(1);
	        	String sql = "REVOKE ALL ON "+sch+"."+tb_name+" FROM NDUSER";
	        	System.out.println(sql);
	        	stmt.execute(sql);
        	}
        	rs.close();
	        

    	}catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static void moveMySQL2CSV(){
		try{
			Class.forName("com.mysql.jdbc.Driver");
			String url = "jdbc:mysql://localhost:3306/shorewarecdr";
    		String username = "root";
    		String password = "password";

    		Connection connection = DriverManager.getConnection(url, username, password);
    		DatabaseMetaData md = connection.getMetaData();
	        ResultSet tbls = md.getTables(null, null, "%", null);
	        Statement stmt = connection.createStatement();
	        String path = "C:/Users/Bryan/Desktop/CSVs_for_Katie/";
	        while(tbls.next()){
	        	FileWriter f = new FileWriter(path+tbls.getString(3)+".csv");  
				BufferedWriter bfw = new BufferedWriter(f);
	        	String q = "SELECT * FROM `"+tbls.getString(3)+"`";
	        	//q += "SELECT * FROM `"+tbls.getString(3)+"` INTO OUTFILE '"+path+tbls.getString(3)+".csv' ";
    			//q += "FIELDS ENCLOSED BY '\"' TERMINATED BY ',' ESCAPED BY '\"' LINES TERMINATED BY '\r\n';";
    			ResultSet row = stmt.executeQuery(q);
    			ResultSetMetaData rsmd = row.getMetaData();
    			int col_count = rsmd.getColumnCount();
    			String header = "";
    			for(int i = 0; i < col_count; i++){
    				header += rsmd.getColumnName(i+1);
    				if(i+1 < col_count){
    					header += ",";
    				}
    			}
    			bfw.write(header+"\n");
    			while(row.next()){
    				String output = "";
    				for(int i = 0; i < col_count; i++){
    					output += row.getString(rsmd.getColumnName(i+1));
    					if(i+1 < col_count){
    						output += ",";
    					}
    				}
    				bfw.write(output+"\n");
    			}
    			bfw.close();
    			f.close();
	        }
	        tbls.close();
	        connection.close();
    	}catch(Exception e){
    		e.printStackTrace();
    	}
	}

	public static void main(String[] args){

		//String sqlite_path = "C:/ProgramData/SynGlyphX/GlyphEd/glyphed.db";
		//String sqlite_path = "C:/Users/Bryan/AppData/Local/SynGlyphX/Glyph Builder - Glyph Viewer/cache/cache_f28a4f1c-c014-4eed-b3f7-1c8e561c954d/sourcedata.db";
		//SQLite2Oracle.readSQLite(sqlite_path,"51c1f523-f0a9-411b-805d-249cce19b526");
		//SQLite2Oracle.readSQLite(sqlite_path,"Composition");
		//SQLite2Oracle.readSQLite(sqlite_path,"Dashboard");
		//SQLite2Oracle.readSQLite(sqlite_path,"GlobalAdmissions");
		SQLite2Oracle.moveMySQL2CSV();
/*
		SQLite2Oracle.revokeAllForSchema("GSMADMIN_INTERNAL");
		SQLite2Oracle.revokeAllForSchema("GSMCATUSER");
		SQLite2Oracle.revokeAllForSchema("GSMUSER");
		SQLite2Oracle.revokeAllForSchema("MDSYS");
		SQLite2Oracle.revokeAllForSchema("OLAPSYS");
		SQLite2Oracle.revokeAllForSchema("ORDSYS");
		SQLite2Oracle.revokeAllForSchema("SYS");
		SQLite2Oracle.revokeAllForSchema("XDB");
		SQLite2Oracle.revokeAllForSchema("XS$NULL");


		try {
			Class.forName("oracle.jdbc.driver.OracleDriver");
		} catch (ClassNotFoundException e) {
			System.out.println("Where is your Oracle JDBC Driver?");
			e.printStackTrace();
			return;
		}
		System.out.println("Oracle JDBC Driver Registered!");
		Connection conn = null;
		try {

			conn = DriverManager.getConnection(
					"jdbc:oracle:thin:@localhost:1521:xe", "nduser", "ndpw");
			System.out.println("Current schema: "+conn.getSchema()+"\n");
			Statement stmt = null;
		    ResultSet rs = null;
		    stmt = conn.createStatement();

            while(rs.next()){
            	System.out.println(rs.getString(1));
            }
            System.out.println("\n"+metaData.getSchemaTerm());
            System.out.println(metaData.getDatabaseProductName());
            rs.close();

            rs = metaData.getTables(null,conn.getSchema(),null,null);

            ResultSet tp = null;
		    while (rs.next()) {
		    	String tSchm = rs.getString(2);
		      	String tName = rs.getString(3);
		      	String tType = rs.getString(4);
		      	System.out.println(tSchm+"."+tName+": "+tType);
		    }
		    stmt.close();
		    conn.close();
		} catch (SQLException e) {
			System.out.println("Connection Failed! Check output console");
			e.printStackTrace();
			return;
		}
*/
	}
}