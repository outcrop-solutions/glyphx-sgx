
import com.almworks.sqlite4java.*;
import java.io.File;

public class MakeSampleDb {

	public static void begin(){

		File file = new File("citiestest.db");
		file.delete();

		SQLiteConnection db = null;
		SQLiteStatement st0 = null;
		SQLiteStatement st1 = null;
		try{
			db = new SQLiteConnection(new File("citiestest.db"));
		    db.open(true);

		    String query = "CREATE TABLE if NOT EXISTS Nodes ";
		    query += "(ID int, CountryCode text, Country text, City text, ";
		    query += "CityFull text, RegionCode text, Region text, ";
		    query += "Population int, Latitude real, Longitude real);";
		    st0 = db.prepare(query); 

			db.exec("BEGIN TRANSACTION;"); 

			st0.step(); 

			st1 = db.prepare("INSERT INTO Nodes VALUES (?,?,?,?,?,?,?,?,?,?);", true); 
			for (int i = 0; i < 100000; i++) { 
				st1.bind(1, i)
				.bind(2, "ad")
				.bind(3, "Brazil")
				.bind(4, "aixirvall")
				.bind(5, "Aixirvall")
				.bind(6, "06")
				.bind(7, "Andorra la Vella")
				.bind(8, 20430)
				.bind(9, 42.5000000000)
				.bind(10, 1.4833333000);
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