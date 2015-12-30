package synglyphx.util;

import java.util.Map;
import java.util.HashMap;

public class ColorIndex {

	private static Map<String, Integer> colorPalette = null;
	
	public ColorIndex(){
		setColorPalette();
	}

	public static void setColorPalette(){
		colorPalette = new HashMap<String,Integer>();
		colorPalette.put("50101101",0);
		colorPalette.put("02550",1);
		colorPalette.put("25500",2);
		colorPalette.put("00255",3);
		colorPalette.put("2552550",4);
		colorPalette.put("1520255",5);
		colorPalette.put("2551680",6);
		colorPalette.put("0255255",7);
		colorPalette.put("2550255",8);
		colorPalette.put("01530",9);
		colorPalette.put("185153102",10);
		colorPalette.put("255180255",11);
		colorPalette.put("0152255",12);
		colorPalette.put("1852550",13);
		colorPalette.put("15200",14);
		colorPalette.put("127127127",15);
		colorPalette.put("127127255",16);
		colorPalette.put("197820",17);
		colorPalette.put("000",18);
		colorPalette.put("255255255",19);
	}

	public static int getIndex(int r, int g, int b){
		String temp = String.valueOf(r)+String.valueOf(g)+String.valueOf(b);
		if(colorPalette.get(temp) == null){
			return 0;
		}
		return colorPalette.get(temp);
	}
}