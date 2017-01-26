
import java.io.*;
import java.util.Scanner;
import synglyphx.io.SDTReader;
import synglyphx.util.BoundingBox;
import synglyphx.io.ParseXY;
import synglyphx.io.Logger;
import synglyphx.util.ErrorHandler;
import synglyphx.user.FilterSetup;

public class GlyphEngine {

	private static SDTReader sdtReader = null;
	private static String sdtPath;
	private static String outDir;
	private static String app;
	private static double[] temp;
	private static boolean f_setup;

	public static int initiate(final String sdt, final String out, final String application){
		sdtPath = sdt;
		outDir = out;
		app = application;
		FilterSetup.getInstance().closeDriverIfOpen();
		try{
			sdtReader = new SDTReader(sdtPath, outDir, application);
		}catch(Exception e){
	 		try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception er){}
	 	}
		return ErrorHandler.getInstance().hasErrors();
	}

	public static void filterSetup(final String sdt){
		FilterSetup.getInstance().addSDTFile(sdt);
	}

	public static boolean isUpdateNeeded(){
		return sdtReader.isUpdateNeeded();
	}

	public static int finishLoading() throws Exception{
		sdtReader.finishLoading();
		return 0;
	}

	public static String[] getErrors(){
		return ErrorHandler.getInstance().getErrors();
	}

	public static int sizeOfQuery(final String id, final String table, final String query){
		int size = sdtReader.sizeOfQuery(id, table, query);
		return size;
	}

	public static String[] distinctValuesForField(final String id, final String table, final String field){

		String[] distincts;
		if(FilterSetup.getInstance().active()){
			distincts = FilterSetup.getInstance().getDistinctValuesForField(id,table,field);
		}else{
			if(!sdtReader.finishedLoading())
				try{
					finishLoading();
				}catch(Exception e){}
			distincts = sdtReader.distinctValuesForField(id,table,field);
		}
		return distincts;
	}

	public static void setQueryForDatasource(final String id, final String table, final String query){

		if(!FilterSetup.getInstance().isCurrentDataID(id)){
			if(!sdtReader.finishedLoading())
				try{
					finishLoading();
				}catch(Exception e){}
		}
		sdtReader.setQueryForDatasource(id, table, query);
	}

	public static int beginGlyphGeneration(){
		try{
			if(!sdtReader.finishedLoading())
				finishLoading();
	 		sdtReader.generateGlyphs();
	 	}catch(Exception e){
	 		try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception er){}
	 	}
	 	return ErrorHandler.getInstance().hasErrors();
	}

	public static String[] getBaseImages(){
		String[] images = sdtReader.getBaseImages();
		Logger.getInstance().add("# of base images: "+String.valueOf(images.length));
		return images;
	}

	public static void setBoundingBox(double[] nw, double[] se, double[] imageSize){
		Logger.getInstance().add("Downloaded Map BoundingBox: ("+String.valueOf(nw[0])+","+String.valueOf(nw[1])+"),("+String.valueOf(se[0])+","+String.valueOf(se[1])+")"); 
		Logger.getInstance().add("Image Size: "+String.valueOf(imageSize[0])+", "+String.valueOf(imageSize[1]));
		sdtReader.setDLMapBoundingBox(new BoundingBox(nw, se), imageSize, temp);
	}

	public static double[] getNWandSE(){
		try{
			if(!sdtReader.finishedLoading())
				finishLoading();
			ParseXY parser = new ParseXY(sdtPath);
			temp = parser.getMinMax(sdtReader.getSourceDataInfo());
		}catch(Exception e){
	 		try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception er){}
	 	}
		Logger.getInstance().add(String.valueOf(temp[2]));
		Logger.getInstance().add(String.valueOf(temp[1]));
		Logger.getInstance().add(String.valueOf(temp[0]));
		Logger.getInstance().add(String.valueOf(temp[3]));
		return temp;
	}

	public static boolean hasImageBeenUpdated(){

		if(app.equals("GlyphViewer")){
			String key = "";
			String check = "";
			try{
				File f = new File(outDir+"/antz/base_image_2.png");
				if(!f.exists()){return true;}
				File dl = new File(outDir+"/antz/DownloadedImageSpecs.txt");
				if(!dl.exists()){return true;}
				Scanner s = new Scanner(new BufferedReader(new FileReader(outDir+"/antz/DownloadedImageSpecs.txt")));
				sdtReader.getDownloadedBaseObject().setCornerString(temp);
				key = sdtReader.getDownloadedBaseObject().getUpdateCheckString();
				check = s.nextLine();
			}catch(Exception e){
		        try{
		            e.printStackTrace(ErrorHandler.getInstance().addError());
		        }catch(Exception ex){}
		        e.printStackTrace();
		    }

			Logger.getInstance().add(check);
			Logger.getInstance().add(key);
			if(key.equals(check) && !key.equals("") && !check.equals("")){
				return false;
			}
		}
		return true;
	}

	public static void main(String [] args){

		//String sdtPath = "C:\\Users\\Bryan\\SharePoint\\Synglyphx Team Site - Documents\\Test Data\\BRYAN\\School Shooting Viz (05-Jan-16).sdt";
	 	//String outDir = "C:\\Users\\Bryan\\SharePoint\\Synglyphx Team Site - Documents\\Test Data\\BRYAN\\Viz";
	 	//String sdtPath = "C:\\Users\\Bryan\\Desktop\\jdbc_test\\official_merged_test.sdt";
	 	//String outDir = "C:\\Users\\Bryan\\Desktop\\jdbc_test\\official merged";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/TooMany Elements SizeBug/Ohio Voter Registration Draft.sdt";
	 	//String outDir = "C:/Users/Bryan/AppData/Local/SynGlyphX/Glyph Builder - Glyph Viewer/cache/cache_2072a4ce-5cf5-4591-84b0-30f87c5cc214";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/WC Data and Files/WC Sample.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/WC Data and Files/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/East Coast Only/East_Coast_Only.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/East Coast Only/Viz";
	 	//String sdtPath = "C:/Users/Bryan/ODBA/1/Synglyphx Team Site - ~1/School Shooting Remap/School Shooting DataMap.sdt";
	 	//String outDir = "C:/Users/Bryan/ODBA/1/Synglyphx Team Site - ~1/School Shooting Remap/School Shooting Portable";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Text Interpolation/text_interpolation_test.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Text Interpolation/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Test New SDT/alases.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Test New SDT/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Link Test Exo/linktest.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Link Test Exo/viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/TELCO Small Subset/Customers Subset.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/TELCO Small Subset/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/GlobalAdmissions20132016/GlobalAdmissions20132016/View 1 Datamap Recommendations.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/GlobalAdmissions20132016/GlobalAdmissions20132016/Viz";
	 	//String sdtPath = "C:\\Users\\Bryan\\Desktop\\Test Files\\working_sqlite\\animatedrotate_planets_noscale.sdt";
	 	//String outDir = "C:\\Users\\Bryan\\Desktop\\Test Files\\working_sqlite";
	 	String sdtPath = "C:/Users/Bryan/Desktop/spectrum_subset.sdt";
	 	String outDir = "C:/Users/Bryan/Desktop/spectrum_subset_data";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Test Files/PercentRank/percentrank.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Test Files/PercentRank/viz";
	 	//String sdtPath = "D:\\Downloads\\Phase 3 Data Sets\\Phase 3 Data Sets\\Poppy Production and Drug Busts.sdt";
	 	//String outDir = "D:\\Downloads\\Phase 3 Data Sets\\Phase 3 Data Sets";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Test Files/working_sqlite/animatedrotate_planets_noscale.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Test Files/working_sqlite";

	 	String first = "C:/ProgramData/SynGlyphX/GlyphEd/Dev/Admissions Officer/Applicants.sdt";
	 	String second = "C:/ProgramData/SynGlyphX/GlyphEd/Spectrum/Spectrum/spectrum.sdt";

	 	String expDir = "GlyphViewer";
	 	GlyphEngine start = new GlyphEngine();
	 	double[] nw = new double[2];
	 	nw[0] = -180.0; nw[1] = 90.0;
	 	double[] se = new double[2];
	 	se[0] = 180.0; se[1] = -90.0;/*
	 	double[] nw = new double[2];
	 	nw[0] = -92.53099621718249; nw[1] = 35.15804917558482;
	 	double[] se = new double[2];
	 	se[0] = -92.3692917828175; se[1] = 34.97863982441518;*/
	 	double[] s = new double[2];
	 	s[0] = 2048.0; s[1] = 1024.0;
	 	//s[0] = 471.0; s[1] = 634.0;
	 	/*
	 	int err = start.initiate(sdtPath, outDir, expDir);

	 	String id = "9a61eaaa-1789-4366-8a31-abc9b95bef73";
	 	String table = "OnlyTable";
	 	String field = "Frequency";
	 	String query = "SELECT * FROM `OnlyTable` WHERE `Frequency` IN ('100.511')";
	 	System.out.println("Distinct Viz1: "+start.distinctValuesForField(id, table, field).length);
	 	System.out.println("sizeOfQuery: "+start.sizeOfQuery(id, table, query));
	 	start.setQueryForDatasource(id, table, query);

	 	start.filterSetup(first);
	 	String id = "0e10b5e1-60fb-4fbc-8e53-0988839dc495";
	 	String table = "ReaderView";
	 	String field = "StaffAssigned";
	 	System.out.println("Distinct Viz1: "+start.distinctValuesForField(id, table, field).length);
	 	int err = start.initiate(first, outDir, expDir);
*/
	 	start.filterSetup(second);
	 	String id = "1e88527e-7da4-4ffd-90cc-d5cf0614ee45";
	 	String table = "WGSData";
	 	String field = "Frequency";
	 	//String query = "SELECT * FROM WGSData WHERE Frequency IN (100.01,100.511,101.012,101.514,102.015,102.516,103.018,103.519,104.02,104.521,105.023,105.524,106.025,106.527,107.028,107.529,108.031,108.532,109.033,109.535,110.036,110.537,111.038,111.54,112.041,112.542,113.044,113.545,114.046,114.548,115.049,115.55,116.051,116.553,117.054,117.555,118.057,118.558,119.059,119.561,120.062,120.563,121.064,121.566,122.067,122.568,123.07,123.571,124.072,124.574,125.075,125.576,126.077,126.579,127.08,127.581,128.083,128.584,129.085,129.587,130.088,130.589,131.09,131.592,132.093,132.594,133.096,133.597,134.098,134.6,135.101,135.602,136.104,136.605,137.106,137.607,138.109,138.61,139.111,139.613,140.114,140.615,141.117,141.618,142.119,142.62,143.122,143.623,144.124,144.626,145.127,145.628,146.13,146.631,147.132,147.633,148.135,148.636,149.137,149.639)";
	 	//System.out.println("Distinct Viz2: "+start.distinctValuesForField(id, table, field).length);
	 	String query = "SELECT * FROM WGSData";
	 	int err = start.initiate(second, outDir, expDir);
	 	System.out.println("sizeOfQuery: "+start.sizeOfQuery(id, table, query));

	 	double st = 0.0;
		double end = 0.0;
		st = System.currentTimeMillis();

	 	start.setQueryForDatasource(id, table, query);

	 	end = System.currentTimeMillis();
		System.out.println("Set Query/Setup Dataframe: "+String.valueOf((end-st)/1000.00));

	 	//double[] nwse = start.getNWandSE();
/*
	 	double st = 0.0;
		double end = 0.0;
		st = System.currentTimeMillis();

	 	String id = "cf127266-5047-40d0-a74c-64e3f462fc75";
	 	String table = "OnlyTable";
	 	String field = "Port";
	 	String[] distinct1 = start.distinctValuesForField(id, table, field);

	 	end = System.currentTimeMillis();
		System.out.println("Port: "+String.valueOf((end-st)/1000.00));

	 	field = "Arrival Time";
	 	String[] distinct2 = start.distinctValuesForField(id, table, field);

	 	end = System.currentTimeMillis();
		System.out.println("Arrival Time: "+String.valueOf((end-st)/1000.00));

		field = "Departure Time";
	 	String[] distinct3 = start.distinctValuesForField(id, table, field);

	 	end = System.currentTimeMillis();
		System.out.println("Departure Time: "+String.valueOf((end-st)/1000.00));

		field = "Vessel Type";
	 	String[] distinct4 = start.distinctValuesForField(id, table, field);

	 	end = System.currentTimeMillis();
		System.out.println("Vessel Type: "+String.valueOf((end-st)/1000.00));*/
	 	/*
	 	for(int i = 0; i < distinct.length; i++)
	 	{
	 		System.out.println((i+1) +": "+ distinct[i]);

	 	}*/
/*
	 	String query = "SELECT * FROM `OnlyTable` WHERE `Department`='Data Wrangler'";
	 	start.setQueryForDatasource(id, table, query);

	 	String id2 = "44088e04-3303-4241-94b0-3b3dac432356";
	 	String table2 = "OnlyTable";
	 	String query2 = "SELECT * FROM `OnlyTable` WHERE `Year` IN ('1997', '1998', '1999', '2000', '2002', '2001')";
	 	start.setQueryForDatasource(id2, table2, query2);

	 	String id = "643cf5b6-d4dc-49bb-a216-69fb092fabf5";
	 	String table = "OnlyTable";
	 	String query = "SELECT * FROM `OnlyTable` WHERE `From` IN ('Mocorito') AND `To` IN ('Sinaloa')";
	 	start.setQueryForDatasource(id, table, query);


	 	String id = "d0179c53-c1ed-4c17-8d50-011327863db8";
	 	String table = "FirstPortion";
	 	String query = "select * from FirstPortion where `OrbitalEcc.` between 0 and 0.2;";
	 	start.setQueryForDatasource(id, table, query);
 	
	 	if(err == 1){
			String [] errors = start.getErrors();
			//System.out.println("Error List:\n");
			for(int i = 0; i < errors.length; i++){
				//System.out.println(errors[i]);
			}
		}
*/
	 	if(start.isUpdateNeeded()){
		 	//double[] nwse = start.getNWandSE();
		 	//start.hasImageBeenUpdated();
		 	start.setBoundingBox(nw,se,s);
		 	
		 	double st1 = 0.0;
			double end1 = 0.0;
			st1 = System.currentTimeMillis();

			err = start.beginGlyphGeneration();

		 	end1 = System.currentTimeMillis();
			System.out.println("Generate Glyphs: "+String.valueOf((end1-st1)/1000.00));

		}
		//String[] images = start.getBaseImages();
		//System.out.println(images.length);

		if(err > 0){
			String [] errors = start.getErrors();
			System.out.println("Error List:\n");
			for(int i = 0; i < errors.length; i++){
				System.out.println(errors[i]);
			}
		}
		
	} 

}