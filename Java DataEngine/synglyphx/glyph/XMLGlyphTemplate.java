package synglyphx.glyph;

import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.util.MapFunction;

public class XMLGlyphTemplate{

	private String label;
	private String id;
	private String tag;
	private String description;
	private String shape;
	private String surface;
	private Double ratio;
	private String topo;
	private String url;
	private HashMap<String,ArrayList<Double>> ranges = null;
	private HashMap<String,String> inputMapping = null;
	private HashMap<String,String> functionMapping = null;
	private HashMap<String,MapFunction> keyValueMap = null;
	private HashMap<String,ArrayList<Double>> minMaxFieldMap = null;
	private int childOf;
	private int branchLevel;
	private int numChildren;
	private int datasource;
	private boolean toMerge;

	public XMLGlyphTemplate(){
		ranges = new HashMap<String, ArrayList<Double>>();
		inputMapping = new HashMap<String,String>();
		functionMapping = new HashMap<String,String>();
		keyValueMap = new HashMap<String,MapFunction>();
		numChildren = 0;
		tag = "No Tag";
		url = "nourl.html";
		description = "";
	}

	public void setToMerge(boolean tm){
		toMerge = tm;
	}

	public boolean getToMerge(){
		return toMerge;
	}

	public HashMap<String, ArrayList<Double>> getRanges(){
		return ranges;
	}

	public boolean minMaxFieldExists(String field){

		boolean isIt = false;
		try{
			if(minMaxFieldMap.get(field) != null){isIt = true;}
		}catch(Exception e){
			isIt = false;
		}

		return isIt;
		
	}

	public ArrayList<Double> getMinMaxField(String field){
		return minMaxFieldMap.get(field);
	}

	public void addMinMaxField(String field, Double min, Double diff){

		if(minMaxFieldMap == null){
			minMaxFieldMap = new HashMap<String,ArrayList<Double>>();
		}
		minMaxFieldMap.put(field, new ArrayList<Double>());
		minMaxFieldMap.get(field).add(min);
		minMaxFieldMap.get(field).add(min+diff);
		
	}

	public void addKeyValue(String field, String type, String def, String key, String value){
		MapFunction mf;

		if(keyValueMap.get(field) == null){
			mf = new MapFunction(type);
			mf.setDefault(def);
		}else{
			mf = keyValueMap.get(field);
		}

		mf.addKey(key);
		mf.addValue(value);
		keyValueMap.put(field, mf);
	}

	public void setDataSource(int ds){
		this.datasource = ds;
	}

	public void setBranchLevel(int bl){
		this.branchLevel = bl;
	}

	public void setID(String id){
		this.id = id;
	}

	public void setShape(String shape){
		this.shape = shape;
	}

	public void setSurface(String surface){
		this.surface = surface;
	}

	public void setRatio(Double ratio){
		this.ratio = ratio;
	}

	public void setTopo(String topo){
		this.topo = topo;
	}

	public void setChildOf(int id){
		this.childOf = id;
	}

	public void addChild(){
		this.numChildren++;
	}

	public void setLabel(String label){
		this.label = label;
	}

	public void setTag(String tag){
		this.tag = tag;
	}

	public void setDescription(String desc){
		this.description = desc;
	}

	public void setURL(String url){
		this.url = url;
	}

	public void addMinMax(String name, Double min, Double max){
		ranges.put(name, new ArrayList<Double>());
		ranges.get(name).add(min);
		ranges.get(name).add(max);
	}

	public void setRGB(String min, String diff, String funct){
		ranges.put("ColorR", new ArrayList<Double>());
		ranges.put("ColorG", new ArrayList<Double>());
		ranges.put("ColorB", new ArrayList<Double>());
		String[] min_data = min.split(",");
		String[] diff_data = diff.split(",");
		double min_r = Double.parseDouble(min_data[0]);
		double min_g = Double.parseDouble(min_data[1]);
		double min_b = Double.parseDouble(min_data[2]);
		ranges.get("ColorR").add(min_r);
		ranges.get("ColorR").add(min_r+Double.parseDouble(diff_data[0]));
		mapFunction("ColorR", funct);
		ranges.get("ColorG").add(min_g);
		ranges.get("ColorG").add(min_g+Double.parseDouble(diff_data[1]));
		mapFunction("ColorG", funct);
		ranges.get("ColorB").add(min_b);
		ranges.get("ColorB").add(min_b+Double.parseDouble(diff_data[2]));
		mapFunction("ColorB", funct);
	}

	public void mapInput(String name, String field){
		if(name.equals("ColorRGB")){
			inputMapping.put("ColorR", field);
			inputMapping.put("ColorG", field);
			inputMapping.put("ColorB", field);
		}else{
			inputMapping.put(name, field);
		}
	}

	public void mapFunction(String name, String function){
		functionMapping.put(name, function);
	}

	public int getDataSource(){
		return datasource;
	}

	public ArrayList<Double> getMinMax(String name){
		return ranges.get(name);
	}

	public int getBranchLevel(){
		return branchLevel;
	}

	public String getInput(String name){
		return inputMapping.get(name);
	}

	public String getFunction(String name){
		return functionMapping.get(name);
	}

	public String getID(){
		return this.id;
	}

	public int getChildOf(){
		return this.childOf;
	}

	public int getNumChildren(){
		return this.numChildren;
	}

	public String getLabel(){
		return this.label;
	}

	public String getTag(){
		return this.tag;
	}

	public String getDescription(){
		return this.description;
	}

	public String getURL(){
		return this.url;
	}

	public String getShape(){
		return this.shape;
	}

	public String getSurface(){
		return this.surface;
	}

	public Double getRatio(){
		return this.ratio;
	}

	public String getTopo(){
		return this.topo;
	}

	public HashMap<String,String> getInputMapping(){
		return inputMapping;
	}

	public HashMap<String,String> getFunctionMapping(){
		return functionMapping;
	}

	public HashMap<String,MapFunction> getKeyValueMap(){
		return keyValueMap;
	}

}