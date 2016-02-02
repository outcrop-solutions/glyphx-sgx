package synglyphx.util;

import java.util.HashMap;
import java.util.Map;
import java.awt.Color;
import java.util.ArrayList;

public class Functions {
	
	public Functions(){

	}

	public static double linearInterpolation(double x1, double x3, double y1, double y3, double x2){
		if(x3-x1 == 0){return 0;}
		return (((x2-x1)*(y3-y1))/(x3-x1))+y1;
	}

	public static double logarithmicInterpolation(double x1, double x3, double y1, double y3, double x2){
		return ((Math.log10(x2-x1+1)*(y3-y1))/Math.log10(x3-x1+1))+y1;
	}

	public static double numericToValue(double x1, MapFunction n2v){
		
		ArrayList<String> temp = n2v.getKeys();
		for(int i=0; i<temp.size();i++){
			if(String.valueOf(Double.parseDouble(temp.get(i))).equals(String.valueOf(x1))){
				return n2v.getValues().get(i);
			}
		}
		return n2v.getDefault();
	}

	public static double textToValue(String x1, MapFunction t2v){

		ArrayList<String> temp = t2v.getKeys();
		for(int i=0; i<temp.size();i++){
			if(temp.get(i).equals(x1)){
				return t2v.getValues().get(i);
			}
		}
		return t2v.getDefault();
	}

	public static double rangeToValue(double x1, MapFunction r2v){
		
		ArrayList<String> temp = r2v.getKeys();
		for(int i=0; i<temp.size();i++){
			String[] data = temp.get(i).split(",");
			Double min = Double.parseDouble(data[0]);
			Double max = Double.parseDouble(data[1]);
			if(x1 >= min && x1 <= max){
				return r2v.getValues().get(i);
			}
		}
		return r2v.getDefault();

	}

	public static double[] convertRGBtoHSV(double red, double green, double blue){

		double[] hsv = new double[3];
		int r = (int)red;
		int g = (int)green;
		int b = (int)blue;
		float[] temp = new float[3];
		temp = Color.RGBtoHSB(r,g,b,temp);
		hsv[0] = (double)temp[0];
		hsv[1] = (double)temp[1];
		hsv[2] = (double)temp[2];

		return hsv;
	}

	public static double[] convertHSVtoRGB(double hue, double sat, double val){

		//System.out.println(String.valueOf(hue)+", "+String.valueOf(sat)+", "+String.valueOf(val));
		double[] rgb = new double[3];
		float h = (float)hue;
		float s = (float)sat;
		float v = (float)val;
		int temp = Color.HSBtoRGB(h,s,v);
		Color color = new Color(temp);
		rgb[0] = (double)color.getRed();
		rgb[1] = (double)color.getGreen();
		rgb[2] = (double)color.getBlue();
		//System.out.println(String.valueOf(rgb[0])+", "+String.valueOf(rgb[1])+", "+String.valueOf(rgb[2]));
		//System.out.println("");
		return rgb;
	}

/*
	public static void main(String[] args){
		double[] hsv = Functions.convertRGBtoHSV(0,255,0);
		System.out.println(hsv[0]);
		System.out.println(hsv[1]);
		System.out.println(hsv[2]);

		double[] rgb = Functions.convertHSVtoRGB(hsv[0],hsv[1],hsv[2]);
		System.out.println(rgb[0]);
		System.out.println(rgb[1]);
		System.out.println(rgb[2]);
	}
*/
}