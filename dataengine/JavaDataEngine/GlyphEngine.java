
import java.io.*;
import java.util.Scanner;
import synglyphx.io.SDTReader;
import synglyphx.util.BoundingBox;
import synglyphx.util.BaseObject;
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
	//Web Viewer only atm
	public static boolean hasDMBaseObject(){
		return sdtReader.hasDMBaseObject();
	}
	//Web Viewer only atm
	public static BaseObject getDMBaseObject(){
		BaseObject temp = sdtReader.getDownloadedBaseObject();
		return temp;
	}

	public static void main(String [] args){

	 	//String sdtPath = "C:\\Users\\Bryan\\Desktop\\Test Files\\working_sqlite\\animatedrotate_planets_noscale.sdt";
	 	//String outDir = "C:\\Users\\Bryan\\Desktop\\Test Files\\working_sqlite";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/spectrum_subset.sdt";
	 	String outDir = "D:/Users/Bryan/Desktop/spectrum_subset_data";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Test Files/PercentRank/percentrank.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Test Files/PercentRank/viz";
	 	//String sdtPath = "C:/ProgramData/SynGlyphX/GlyphEd/Spectrum/Spectrum/spectrum_nodata.sdt";
	 	String sdtPath = "C:/Users/Bryan/Documents/spectrum_subset_test.sdt";

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
	 	
	 	int err = start.initiate(sdtPath, outDir, expDir);

	 	String id = "d1bc6801-7ebd-4b57-9a84-716917b7f972";
	 	String table = "OnlyTable";
	 	String field = "Frequency";
	 	String query = "SELECT * FROM `OnlyTable` WHERE `Frequency` IN (100.01,100.511,101.012,101.514)";
	 	System.out.println("Distinct Viz1: "+start.distinctValuesForField(id, table, field).length);
	 	System.out.println("sizeOfQuery: "+start.sizeOfQuery(id, table, query));
	 	//start.setQueryForDatasource(id, table, query);
/*
	 	start.filterSetup(first);
	 	String id = "0e10b5e1-60fb-4fbc-8e53-0988839dc495";
	 	String table = "ReaderView";
	 	String field = "StaffAssigned";
	 	String query = "SELECT * FROM ReaderView WHERE StaffAssigned IN ('Alisa Fisher')";
	 	System.out.println("Distinct Viz1: "+start.distinctValuesForField(id, table, field).length);
	 	//int err = start.initiate(sdtPath, outDir, expDir);
	 	
	 	start.filterSetup(sdtPath);
	 	String id = "1e88527e-7da4-4ffd-90cc-d5cf0614ee45";
	 	String table = "SpectrumData";
	 	String field = "Frequency";
	 	//String query = "SELECT * FROM WGSData WHERE Frequency IN (100.01,100.511,101.012,101.514,102.015,102.516,103.018,103.519,104.02,104.521,105.023,105.524,106.025,106.527,107.028,107.529,108.031,108.532,109.033,109.535,110.036,110.537,111.038,111.54,112.041,112.542,113.044,113.545,114.046,114.548,115.049,115.55,116.051,116.553,117.054,117.555,118.057,118.558,119.059,119.561,120.062,120.563,121.064,121.566,122.067,122.568,123.07,123.571,124.072,124.574,125.075,125.576,126.077,126.579,127.08,127.581,128.083,128.584,129.085,129.587,130.088,130.589,131.09,131.592,132.093,132.594,133.096,133.597,134.098,134.6,135.101,135.602,136.104,136.605,137.106,137.607,138.109,138.61,139.111,139.613,140.114,140.615,141.117,141.618,142.119,142.62,143.122,143.623,144.124,144.626,145.127,145.628,146.13,146.631,147.132,147.633,148.135,148.636,149.137,149.639)";
	 	//System.out.println("Distinct Viz2: "+start.distinctValuesForField(id, table, field).length);
	 	String query = "SELECT * FROM SpectrumData WHERE Frequency IN (100.01,100.511,101.012,101.514)";
	 	int err = start.initiate(second, outDir, expDir);

	 	System.out.println("sizeOfQuery: "+start.sizeOfQuery(id, table, query));
*/
	 	double st = 0.0;
		double end = 0.0;
		st = System.currentTimeMillis();

	 	start.setQueryForDatasource(id, table, query);

	 	end = System.currentTimeMillis();
		System.out.println("Set Query/Setup Dataframe: "+String.valueOf((end-st)/1000.00));

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
		
	} 

}