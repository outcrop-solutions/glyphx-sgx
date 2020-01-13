
import java.io.*;
import java.util.Scanner;
import synglyphx.io.SDTReader;
import synglyphx.util.BoundingBox;
import synglyphx.util.BaseObject;
import synglyphx.io.ParseXY;
import synglyphx.io.Logger;
import synglyphx.util.ErrorHandler;
import synglyphx.user.FilterSetup;
import synglyphx.user.FileManager;

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

	public static String[] getCompassValues(){
		String[] compass = sdtReader.getCompassValues();
		return compass;
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

	public static boolean downloadFiles(final String bucket_name, final String file_name, final String location){

		Logger.getInstance().add("DOWNLOAD FILES");
		Logger.getInstance().add(bucket_name);
		Logger.getInstance().add(file_name);
		Logger.getInstance().add(location);

		boolean success = false;
		FileManager fm = new FileManager();
		success = fm.downloadFile(bucket_name, file_name, location);
		
		Logger.getInstance().add(Boolean.toString(success));

		return success;
	}

	public static void main(String [] args){

	 	GlyphEngine start = new GlyphEngine();
	 	start.downloadFiles("viz-group-notredame-source", "Prospect and Segmentation/Prospects Only.sdt", "C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Release/VizFiles/");
	 	start.downloadFiles("viz-group-notredame-source", "Prospect and Segmentation/Color Legend Breakdown.jpg", "C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Release/VizFiles/");
	 	start.downloadFiles("viz-group-notredame-source", "Prospect and Segmentation/Prospects Only Legend with Term.png", "C:/Users/bryan/OneDrive/Documents/GitHub/sgx/bin/Win64/Release/VizFiles/");
		
	} 

}