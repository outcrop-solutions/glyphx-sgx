
import java.io.*;
import java.util.Arrays;
import java.util.ArrayList;
import com.almworks.sqlite4java.*;

public class DbReader {

	private ArrayList<String> tableNames = null;
	private int headers = 0;

	public DbReader(){

	}

	public void readTables(String path){

		tableNames = new ArrayList<String>();
		SQLiteStatement st0 = null;
		try{
			SQLiteConnection db = new SQLiteConnection(new File(path));
			db.open(true);

		    st0 = db.prepare("SELECT name FROM sqlite_master WHERE type='table';"); 

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			int size = st0.columnCount();
			while(st0.hasRow()){
				String str = "";
				str += st0.columnValue(0);
				tableNames.add(str);
				headers++;
				st0.step();
			}

			db.exec("COMMIT TRANSACTION;"); 

		}catch(SQLiteException se){
			se.printStackTrace();
		}
	}

	private void readSQLite(String path, String name){

		ArrayList<String> temp = null;

		SQLiteStatement st0 = null;
		try{
			SQLiteConnection db = new SQLiteConnection(new File(path));
			db.open(true);

		    st0 = db.prepare("SELECT * FROM '"+name+"';"); 

			db.exec("BEGIN TRANSACTION;"); 
			st0.step(); 

			String str = "";
			int size = st0.columnCount();
			for(int i = 0; i<size; i++){
				str += st0.getColumnName(i)+" | ";
			}
			System.out.println(str+" "+String.valueOf(size));

			int stop = 0;
			while(st0.hasRow()){
				str = "";
				for(int i = 0; i<size; i++){
					str += st0.columnValue(i)+" | ";
				}
				System.out.println(str);
				st0.step();
				if(stop == 10){break;}
				stop++;
			}

			db.exec("COMMIT TRANSACTION;"); 
			db.dispose();

		}catch(SQLiteException se){
			se.printStackTrace();
		}
	}

	public ArrayList<String> getTableNames(){
		return tableNames;
	}

	public static void main(String [] args){
		DbReader reader = new DbReader();
		reader.readTables("C:\\Users\\Bryan\\AppData\\Local\\SynGlyphX\\Glyph Builder - Glyph Viewer\\cache\\cache_35214023-cc10-4b71-abfc-5115e7af4df5\\sourcedata.db");
		//reader.readTables("C:\\Users\\Bryan\\Desktop\\cache_35214023-cc10-4b71-abfc-5115e7af4df5\\sourcedata.db");
		//reader.readTables("C:\\Users\\Bryan\\Desktop\\New folder (2)\\sourcedata.db");
		for(int i=0;i<reader.getTableNames().size();i++){
			reader.readSQLite("C:\\Users\\Bryan\\AppData\\Local\\SynGlyphX\\Glyph Builder - Glyph Viewer\\cache\\cache_35214023-cc10-4b71-abfc-5115e7af4df5\\sourcedata.db",reader.getTableNames().get(i));
			//reader.readSQLite("C:\\Users\\Bryan\\Desktop\\cache_35214023-cc10-4b71-abfc-5115e7af4df5\\sourcedata.db",reader.getTableNames().get(i));
			//reader.readSQLite("C:\\Users\\Bryan\\Desktop\\New folder (2)\\sourcedata.db",reader.getTableNames().get(i));
		}
		System.out.println(reader.getTableNames().size());
	}
}