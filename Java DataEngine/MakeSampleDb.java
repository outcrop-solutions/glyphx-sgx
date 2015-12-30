
import com.almworks.sqlite4java.*;
import java.io.File;

public class MakeSampleDb {

	public static void begin(){

		File file = new File("temp.sqlite");
		file.delete();

		SQLiteConnection db = null;
		SQLiteStatement st0 = null;
		SQLiteStatement st1 = null;
		try{
			db = new SQLiteConnection(new File("temp.sqlite"));
		    db.open(true);

		    String query = "CREATE TABLE if NOT EXISTS Nodes (id int, line int, child_count int, parent_id int);";
		    st0 = db.prepare(query); 

			db.exec("BEGIN TRANSACTION;"); 

			st0.step(); 

			st1 = db.prepare("INSERT INTO Nodes VALUES (?,?,?,?);", true); 
			for (int i = 0; i < 4; i++) { 
				st1.bind(1, i)
				.bind(2, i)
				.bind(3, i)
				.bind(4, i);
				st1.step(); 
				st1.reset();
			} 
			db.exec("COMMIT TRANSACTION;"); 
			db.dispose(); 
		}catch(SQLiteException se){
			se.printStackTrace();
		}

		commitEdges();
	}

	public static void commitEdges(){

		SQLiteConnection db = null;
		SQLiteStatement st0 = null;
		SQLiteStatement st1 = null;
		try{
			db = new SQLiteConnection(new File("temp.sqlite"));
		    db.open(true);

			String query = "CREATE TABLE if NOT EXISTS Edges (parentid int, childid int, type int)";
			st0 = db.prepare(query); 

			db.exec("BEGIN TRANSACTION;"); 

			st0.step();

			st1 = db.prepare("INSERT INTO Edges VALUES (?,?,?);", true); 
			for (int i = 0; i < 4; i++) { 
				st1.bind(1, i)
				.bind(2, i)
				.bind(3, i);
				st1.step();
				st1.reset();
			}
			db.exec("COMMIT TRANSACTION;"); 

			db.dispose(); 
		}catch(SQLiteException se){
			se.printStackTrace();
		} 
	}

	public static void main(String [] args){
		begin();
	}
}