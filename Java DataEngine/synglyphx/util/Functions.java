package synglyphx.util;

import java.util.HashMap;
import java.util.Map;
import java.util.ArrayList;

public class Functions {
	
	public Functions(){

	}

	public static double linearInterpolation(double x1, double x3, double y1, double y3, double x2){
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

}