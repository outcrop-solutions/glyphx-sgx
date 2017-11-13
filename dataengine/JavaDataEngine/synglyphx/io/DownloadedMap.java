package synglyphx.io;

import java.util.ArrayList;
import java.util.Collections;
import java.io.File;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.net.URL;
import java.net.MalformedURLException;
import java.awt.Color;
import java.awt.image.BufferedImage;
import javax.imageio.ImageIO;
import synglyphx.util.BaseObject;

public class DownloadedMap {

	private static double PI = 3.14159;
	private static int MAXZOOMLEVEL = 18;
	private static double METERSPERPIXELATZOOM0 = 156543.034;
	private static double EARTH_RADIUS_IN_METERS = 6372798.2;
	private static double METERSPERDEGREELONGITUDE = 111319.892;
	private static double METERSPERDEGREELATITUDE = 110540.0;
	private static double DEG_TO_RAD = PI / 180.0;
	private static String MAPQUESTKEY = "Fmjtd%7Cluur2du8nl%2C2l%3Do5-9arx0r";

	private BaseObject map;
	private double long_min;
	private double long_max;
	private double lat_min;
	private double lat_max;
	private double[] newImageSize;
	private double[] newNorthWest;
	private double[] newSouthEast;

	public DownloadedMap(BaseObject map, double[] nwse){
		this.map = map;
		this.long_min = nwse[0];
		this.long_max = nwse[2];
		this.lat_min = nwse[3];
		this.lat_max = nwse[1];
	}

	public boolean downloadMap(String outpath) {
		
		double[] imageSize = map.getImageSize();
		int zoomLevel = getZoomLevel();
		System.out.println("Zoom Level: "+String.valueOf(zoomLevel));

		if (map.getBestFit().equals("true")) {

			imageSize[0] += 2 * Integer.parseInt(map.getMargin());
			imageSize[1] += 2 * Integer.parseInt(map.getMargin());

			int maxSize = 3840;
			imageSize[0] = Math.min(imageSize[0], maxSize);
			imageSize[1] = Math.min(imageSize[1], maxSize);
		}

		double cosineAtCenter = Math.cos(getCenter(lat_min, lat_max) * DEG_TO_RAD);
		double metersPerPixelAtZoom = (METERSPERPIXELATZOOM0 / Math.pow(2.0, zoomLevel)) * cosineAtCenter;

		double[] center = new double[]{getCenter(lat_min, lat_max), getCenter(long_min, long_max)};
		String imageUrl = generateMapQuestOpenString(center, zoomLevel, map);
		Logger.getInstance().add(imageUrl);

		try{
			URL url = new URL(imageUrl);
			InputStream in = new BufferedInputStream(url.openStream());
			ByteArrayOutputStream out = new ByteArrayOutputStream();
			byte[] buf = new byte[1024];
			int n = 0;
			while (-1!=(n=in.read(buf)))
			{
			   out.write(buf, 0, n);
			}
			out.close();
			in.close();
			byte[] response = out.toByteArray();

			if (map.getGrayScale().equals("true")) {
				response = convertToGrayScale(response);
			}
			if (map.getInvert().equals("true")) {
				response = invertImage(response);
			}

			FileOutputStream fos = new FileOutputStream(outpath+"/base_image.png");
			fos.write(response);
			fos.close();
		}
		catch(FileNotFoundException fnfe){
			fnfe.printStackTrace();
		}
		catch(MalformedURLException mue){
			mue.printStackTrace();
		}
		catch(IOException ioe){
			ioe.printStackTrace();
		}

		double lonRadius = (metersPerPixelAtZoom  * (imageSize[0] / 2.0)) / (METERSPERDEGREELONGITUDE  * cosineAtCenter);
		double latRadius = (metersPerPixelAtZoom * (imageSize[1] / 2.0)) / METERSPERDEGREELATITUDE;
		newNorthWest = new double[]{center[1] - lonRadius, center[0] + latRadius};
		newSouthEast = new double[]{center[1] + lonRadius, center[0] - latRadius};

		return true;
	}

	private int getZoomLevel() {

		double cosineAtCenter = Math.cos(getCenter(lat_min, lat_max) * DEG_TO_RAD);
		double[] imageSize = map.getImageSize();
		double hDistanceInMeters = Math.abs(distanceInMeters(new double[]{long_min,getCenter(lat_min, lat_max)},new double[]{long_max,getCenter(lat_min, lat_max)}));
		if (Math.abs(long_max-long_min) > 180.0) {

			hDistanceInMeters = (EARTH_RADIUS_IN_METERS * cosineAtCenter * 2.0 * PI) - hDistanceInMeters;
		}
		double vDistanceInMeters = Math.abs(distanceInMeters(new double[]{getCenter(long_min, long_max), lat_min}, new double[]{getCenter(long_min, long_max), lat_max}));
		System.out.println("hDistanceInMeters: " + String.valueOf(hDistanceInMeters));
		System.out.println("vDistanceInMeters: " + String.valueOf(vDistanceInMeters));

		if (map.getBestFit().equals("true")) {

			for (int i = MAXZOOMLEVEL; i > 0; --i) {

				double metersPerPixelAtZoom = (METERSPERPIXELATZOOM0 / Math.pow(2.0, i)) * cosineAtCenter;
				double[] imageSizeAtZoom = new double[2];
				imageSizeAtZoom[0] = hDistanceInMeters / metersPerPixelAtZoom;
				imageSizeAtZoom[1] = vDistanceInMeters / metersPerPixelAtZoom;
				if ((imageSizeAtZoom[0] < imageSize[0]) && (imageSizeAtZoom[1] < imageSize[1])) {

					imageSize[0] = Math.ceil(imageSizeAtZoom[0]);
					imageSize[1] = Math.ceil(imageSizeAtZoom[1]);
					newImageSize = imageSize;
					//System.out.println("Image Size: " + String.valueOf(imageSize[0]) + ", " + String.valueOf(imageSize[1]));
					return i;
				}
			}
		}

		double hZoomLevel = Math.log((METERSPERPIXELATZOOM0 * cosineAtCenter) / (hDistanceInMeters / imageSize[0])) / Math.log(2.0); // / cosineAtCenter);
		double vZoomLevel = Math.log((METERSPERPIXELATZOOM0 * cosineAtCenter) / (vDistanceInMeters / imageSize[1])) / Math.log(2.0);

		//Zoom level can never go above 18 on MapQuest Open
		return Math.min((int)Math.min(hZoomLevel, vZoomLevel), MAXZOOMLEVEL);
	}

	private String generateMapQuestOpenString(double[] center, int zoomLevel, BaseObject map) {

		String url = "http://open.mapquestapi.com/staticmap/v4/getmap?key=" + MAPQUESTKEY;
		url += "&center=" + String.valueOf(center[0]) + "," + String.valueOf(center[1]);
		url += "&zoom=" + String.valueOf(zoomLevel) + "&size=" + String.valueOf((int)map.getImageSize()[0]) + "," + String.valueOf((int)map.getImageSize()[1]);
		url += "&type=";
		if (map.getMapType().equals("Map")) {
			url += "map";
		}
		else if (map.getMapType().equals("Satellite")) {
			url += "sat";
		}
		else {
			url += "hyb";
		}
		url += "&imagetype=png";
		url += "&pois=";

		return url;
	}

	private double arcInRadians(double[] from, double[] to) {
		double latitudeArc=(from[1]-to[1])*DEG_TO_RAD;
		double longitudeArc=(from[0]-to[0])*DEG_TO_RAD;
		double latitudeH=Math.sin(latitudeArc*0.5);
		latitudeH*=latitudeH;
		double lontitudeH=Math.sin(longitudeArc*0.5);
		lontitudeH*=lontitudeH;
		double tmp=Math.cos(from[1]*DEG_TO_RAD)*Math.cos(to[1]*DEG_TO_RAD);
		return 2.0*Math.asin(Math.sqrt(latitudeH+tmp*lontitudeH));
	}

	private double distanceInMeters(double[] from,double[] to) { 
		return EARTH_RADIUS_IN_METERS*arcInRadians(from, to);
	}

	private double getCenter(double min, double max) {
		return ((min+max)/2);
	}

	private byte[] convertToGrayScale(byte[] response) {

		try {
			ByteArrayInputStream input = new ByteArrayInputStream(response);
			BufferedImage image = ImageIO.read(input);
			int width = image.getWidth();
			int height = image.getHeight();

			for(int i=0; i<height; i++){

				for(int j=0; j<width; j++){

					Color c = new Color(image.getRGB(j, i));
					int red = (int)(c.getRed() * 0.299);
					int green = (int)(c.getGreen() * 0.587);
					int blue = (int)(c.getBlue() *0.114);
					Color newColor = new Color(red+green+blue,

					red+green+blue,red+green+blue);

					image.setRGB(j,i,newColor.getRGB());
				}
			}

			ByteArrayOutputStream output = new ByteArrayOutputStream();
			ImageIO.write(image, "png", output);
			return output.toByteArray();
		} 
		catch (Exception e) {}
		return response;
	}

	private byte[] invertImage(byte[] response) {

        try {
        	ByteArrayInputStream input = new ByteArrayInputStream(response);
            BufferedImage inputFile = ImageIO.read(input);

	        for (int x = 0; x < inputFile.getWidth(); x++) {
	            for (int y = 0; y < inputFile.getHeight(); y++) {
	                int rgba = inputFile.getRGB(x, y);
	                Color col = new Color(rgba, true);
	                col = new Color(255 - col.getRed(),
	                                255 - col.getGreen(),
	                                255 - col.getBlue());
	                inputFile.setRGB(x, y, col.getRGB());
	            }
	        }

	        ByteArrayOutputStream output = new ByteArrayOutputStream();
			ImageIO.write(inputFile, "png", output);
			return output.toByteArray();
		}
		catch (Exception e) {}
		return response;
	}

	public double[] northWestCorner(){
		return newNorthWest;
	}

	public double[] southEastCorner(){
		return newSouthEast;
	}

	public double[] imageSize(){
		return newImageSize;
	}
}