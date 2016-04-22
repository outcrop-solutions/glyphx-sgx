
import java.io.*;
import java.util.Scanner;
import synglyphx.io.SDTReader;
import synglyphx.util.BoundingBox;
import synglyphx.io.ParseXY;
import synglyphx.io.Logger;

public class GlyphEngine {

	private static SDTReader sdtReader = null;
	private static String sdtPath;
	private static String outDir;
	private static String app;
	private static double[] temp;

	public static void initiate(final String sdt, final String out, final String application){
		sdtPath = sdt;
		outDir = out;
		app = application;
		sdtReader = new SDTReader(sdtPath, outDir, application);
	}

	public static boolean isUpdateNeeded(){
		return sdtReader.isUpdateNeeded();
	}

	public static boolean beginGlyphGeneration(){
		try{
	 		sdtReader.generateGlyphs();
	 	//}catch(OutOfMemoryError e){
	 	}catch(Exception e){
	 		e.printStackTrace();
	 		Logger.getInstance().add(e.getMessage());
	 		return false;
	 	}
	 	return true;
	}

	public static String[] getBaseImages(){
		String[] images = sdtReader.getBaseImages();
		return images;
	}

	public static void setBoundingBox(double[] nw, double[] se, double[] imageSize){
		Logger.getInstance().add("Downloaded Map BoundingBox: ("+String.valueOf(nw[0])+","+String.valueOf(nw[1])+"),("+String.valueOf(se[0])+","+String.valueOf(se[1])+")"); 
		Logger.getInstance().add("Image Size: "+String.valueOf(imageSize[0])+", "+String.valueOf(imageSize[1]));
		sdtReader.setDLMapBoundingBox(new BoundingBox(nw, se), imageSize, temp);
	}

	public static double[] getNWandSE(){
		ParseXY parser = new ParseXY(sdtPath);
		temp = parser.getMinMax(sdtReader.getSourceDataInfo());
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
			}catch(IOException ioe){
				ioe.printStackTrace();
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
	 	//String sdtPath = "C:\\Users\\Bryan\\Desktop\\test_for_ray\\url_and_description.sdt";
	 	//String outDir = "C:\\Users\\Bryan\\Desktop\\test_for_ray\\Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/East Coast Only/East_Coast_Only.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/East Coast Only/Viz";
	 	//String sdtPath = "C:/Users/Bryan/Desktop/Links Test/link_test.sdt";
	 	//String outDir = "C:/Users/Bryan/Desktop/Links Test/viz";
	 	String sdtPath = "C:/Users/Bryan/Desktop/ND Bug 2/Total Picture DataMap.sdt";
	 	String outDir = "C:/Users/Bryan/Desktop/ND Bug 2/Viz";
	 	
	 	String expDir = "DataMapper";
	 	GlyphEngine start = new GlyphEngine();
	 	double[] nw = new double[2];
	 	nw[0] = -160.0; nw[1] = 160.0;
	 	double[] se = new double[2];
	 	se[0] = 160.0; se[1] = -160.0;/*
	 	double[] nw = new double[2];
	 	nw[0] = -85.25350956479433; nw[1] = 41.618268866536965;
	 	double[] se = new double[2];
	 	se[0] = -79.62852983520567; se[1] = 39.46589663346303;*/
	 	double[] s = new double[2];
	 	s[0] = 2048.0; s[1] = 1024.0;

	 	start.initiate(sdtPath, outDir, expDir);

	 	if(start.isUpdateNeeded()){
		 	//double[] nwse = start.getNWandSE();
		 	//start.hasImageBeenUpdated();
		 	start.setBoundingBox(nw,se,s);
		 	
		 	start.beginGlyphGeneration();

		}
		String[] images = start.getBaseImages();
		System.out.println(images.length);
	} 

}