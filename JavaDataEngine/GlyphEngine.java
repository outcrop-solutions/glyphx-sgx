
import java.io.*;
import java.util.Scanner;
import synglyphx.io.SDTReader;
import synglyphx.util.BoundingBox;
import synglyphx.io.ParseXY;
import synglyphx.io.Logger;
import synglyphx.util.ErrorHandler;

public class GlyphEngine {

	private static SDTReader sdtReader = null;
	private static String sdtPath;
	private static String outDir;
	private static String app;
	private static double[] temp;

	public static int initiate(final String sdt, final String out, final String application){
		sdtPath = sdt;
		outDir = out;
		app = application;
		try{
			sdtReader = new SDTReader(sdtPath, outDir, application);
		}catch(Exception e){
	 		try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception er){}
	 	}
		return ErrorHandler.getInstance().hasErrors();
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

	public static String[] distinctValuesForField(final String id, final String table, final String field){
		String[] distincts = sdtReader.distinctValuesForField(id,table,field);
		return distincts;
	}

	public static void setQueryForDatasource(final String id, final String table, final String query){
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
	 	String sdtPath = "C:/Users/Bryan/Desktop/Test New SDT/alases.sdt";
	 	String outDir = "C:/Users/Bryan/Desktop/Test New SDT/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Link Test Exo/linktest.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Link Test Exo/viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/TELCO Small Subset/Customers Subset.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/TELCO Small Subset/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/GlobalAdmissions20132016/GlobalAdmissions20132016/View 1 Datamap Recommendations.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/GlobalAdmissions20132016/GlobalAdmissions20132016/Viz";
	 	
	 	String expDir = "DataMapper";
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
/*
	 	String id = "8f101839-db92-4f15-8dd9-375f223e48f1";
	 	String table = "FirstPortion";
	 	String field = "SpectralType";
	 	String[] distinct = start.distinctValuesForField(id, table, field);
	 	for(int i = 0; i < distinct.length; i++)
	 	{
	 		System.out.println((i+1) +": "+ distinct[i]);
	 	}

	 	String query = "select * from FirstPortion where `OrbitalEcc.` between 0 and 0.2;";
	 	start.setQueryForDatasource(id, table, query);
*/	 	
	 	if(err == 1){
			String [] errors = start.getErrors();
			//System.out.println("Error List:\n");
			for(int i = 0; i < errors.length; i++){
				//System.out.println(errors[i]);
			}
		}

	 	if(start.isUpdateNeeded()){
		 	//double[] nwse = start.getNWandSE();
		 	//start.hasImageBeenUpdated();
		 	start.setBoundingBox(nw,se,s);
		 	
		 	err = start.beginGlyphGeneration();

		}
		String[] images = start.getBaseImages();
		System.out.println(images.length);

		if(err == 1){
			String [] errors = start.getErrors();
			//System.out.println("Error List:\n");
			for(int i = 0; i < errors.length; i++){
				//System.out.println(errors[i]);
			}
		}
		
	} 

}