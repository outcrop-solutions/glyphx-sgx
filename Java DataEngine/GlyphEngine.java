
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

	public static void beginGlyphGeneration(){
		sdtReader.generateGlyphs();
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
				Scanner s = new Scanner(new BufferedReader(new FileReader(outDir+"\\antz\\DownloadedImageSpecs.txt")));
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

	 	String sdtPath = "C:\\Users\\Bryan\\Desktop\\East Coast Only\\East_Coast_Only.sdt";
	 	String outDir = "C:\\Users\\Bryan\\AppData\\Local\\SynGlyphX\\Glyph Builder - Glyph Viewer\\cache\\cache_00501cde-0d46-4db3-b098-f1bb812b9a71";
	 	String expDir = "GlyphViewer";
	 	GlyphEngine start = new GlyphEngine();/*
	 	double[] nw = new double[2];
	 	nw[0] = -85.53713495809289; nw[1] = 44.63280303516512;
	 	double[] se = new double[2];
	 	se[0] = -69.16756504190711; se[1] = 25.01479696483488;
	 	double[] s = new double[2];
	 	s[0] = 745.0; s[1] = 1080.0;*/
	 	start.initiate(sdtPath, outDir, expDir);
	 	double[] nwse = start.getNWandSE();
	 	start.hasImageBeenUpdated();
	 	//start.setBoundingBox(nw,se,s);
	 	//start.beginGlyphGeneration();
	} 

}