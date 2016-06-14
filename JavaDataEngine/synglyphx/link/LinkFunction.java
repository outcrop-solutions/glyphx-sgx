package synglyphx.link;

import java.util.HashMap;

public class LinkFunction {

	enum Function {
		MATCHVALUE, KEYVALUE, KEYRANGE;
	}

	private Function type = null;
	private HashMap<String, String> key_value_pairs;
	private HashMap<String, double[]> key_range_pairs; 

	public LinkFunction(String name){
		switch(name){
			case "Match Value": type = Function.MATCHVALUE;
						break;
			case "Key to Value": type = Function.KEYVALUE; 
						key_value_pairs = new HashMap<String, String>();
						break;
			case "Key to Range": type = Function.KEYRANGE;
						key_range_pairs = new HashMap<String, double[]>();
						break;
			default: type = null;
						break;
		}
	}

	public HashMap<String, String> getKeyValuePairs(){
		return key_value_pairs;
	}

	public HashMap<String, double[]> getKeyRangePairs(){
		return key_range_pairs;
	}

	public void addKeyValuePair(String key, String value){
		if(type != Function.KEYVALUE){return;}
		key_value_pairs.put(key, value);
	}

	public void addKeyRangePair(String key, String min, String max){
		if(type != Function.KEYRANGE){return;}
		double[] temp = new double[2];
		temp[0] = Double.parseDouble(min); temp[1] = Double.parseDouble(max);
		key_range_pairs.put(key, temp);
	}

	public Function type(){
		return type;
	}
}