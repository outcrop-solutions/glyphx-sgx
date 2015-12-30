package synglyphx.util;

import java.util.ArrayList;

public class MapFunction {

	private double _default;
	private String type;

	private ArrayList<String> keys;
	private ArrayList<Double> values;
	
	public MapFunction(String type){
		this.type = type;
		keys = new ArrayList<String>();
		values = new ArrayList<Double>();
	}

	public void setDefault(String def){
		_default = Double.parseDouble(def);
	}

	public void addKey(String key){
		keys.add(key);
	}

	public void addValue(String value){
		values.add(Double.parseDouble(value));
	}

	public double getDefault(){
		return _default;
	}

	public ArrayList<String> getKeys(){
		return keys;
	}

	public ArrayList<Double> getValues(){
		return values;
	}

	public String getType(){
		return type;
	}

	public String kvToString(){

		String temp = "";
		for(int i=0;i<keys.size();i++){
			temp += "{key: "+keys.get(i)+", value: "+String.valueOf(values.get(i))+"}";
		}

		return temp;
	}

}