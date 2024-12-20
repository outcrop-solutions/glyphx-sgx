package synglyphx.glyph;

import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.util.GeoID;
import synglyphx.util.Functions;
import synglyphx.util.MapFunction;
import synglyphx.data.FieldProperties;

public class XMLGlyphTemplate{

	private int label;
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
	private HashMap<String,double[]> riBounds = null; //range interpolation constraints
	private int childOf;
	private int branchLevel;
	private int numChildren;
	private int datasource;
	private int lastChildID;
	private boolean toMerge;
	private FieldProperties fieldProperties = null;

	public XMLGlyphTemplate(){
		ranges = new HashMap<String, ArrayList<Double>>();
		inputMapping = new HashMap<String,String>();
		functionMapping = new HashMap<String,String>();
		keyValueMap = new HashMap<String,MapFunction>();
		riBounds = new HashMap<String, double[]>();
		numChildren = 0;
		tag = "No Tag";
		url = "nourl.html";
		description = "";
	}
	public void setLastChildID(int lastChild){
		lastChildID = lastChild;
	}

	public int getLastChildID(){
		return lastChildID;
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
		if(minMaxFieldMap == null){return false;}
		if(minMaxFieldMap.containsKey(field)){
			return true;
		}
		return false;
	}

	public void setRangeInterpolationConstraints(String field, String ib, String db, String ia, String da){

		double[] bounds = new double[4];
		bounds[0] = Double.parseDouble(ib);
		bounds[1] = Double.parseDouble(db);
		bounds[2] = Double.parseDouble(ia);
		bounds[3] = Double.parseDouble(da);
		riBounds.put(field, bounds);
	}

	public boolean hasRangeBoundForField(String field){
		if(riBounds.containsKey(field)){
			return true;
		}
		return false;
	}

	public double[] getRangeInterpolationConstraints(String field){
		return riBounds.get(field);
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

	public void updateMinMaxField(String field, Double min, Double max){
		minMaxFieldMap.put(field, new ArrayList<Double>());
		minMaxFieldMap.get(field).add(min);
		minMaxFieldMap.get(field).add(max);
	}

	public void defaultSetter(String field, String type, String def){
		GeoID geo = new GeoID();
		if(field.equals("ColorRGB")){
			String[] defSplit = def.split(",");
			double[] defhsv = Functions.convertRGBtoHSV(Double.parseDouble(defSplit[0]),Double.parseDouble(defSplit[1]),Double.parseDouble(defSplit[2]));
			setDefault("ColorR", type, String.valueOf(defhsv[0]));
			setDefault("ColorG", type, String.valueOf(defhsv[1]));
			setDefault("ColorB", type, String.valueOf(defhsv[2]));
		}else if(field.equals("GeometryShape")){
			setDefault(field, type, String.valueOf(geo.getValue(def,this.surface)));
		}else if(field.equals("VirtualTopologyType")){
			setDefault(field, type, String.valueOf(geo.getTopoValue(def)));
		}else{
			setDefault(field, type, def);
		}

	}

	public void setDefault(String field, String type, String def){
		MapFunction mf = new MapFunction(type);
		mf.setDefault(def);
		keyValueMap.put(field, mf);
	}

	public void addKeyValue(String field, String key, String value){
		keyValueMap.get(field).addKey(key);
		keyValueMap.get(field).addValue(value);
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

	public void setLabel(int l){
		label = l;
	}

	public void setTag(String tag, String tag_id){
		if(tag_id.charAt(0) == '~'){
			this.tag = tag;
		}else{
			this.tag = tag_id;
		}
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
		double[] min_hsv = Functions.convertRGBtoHSV(Double.parseDouble(min_data[0]),Double.parseDouble(min_data[1]),Double.parseDouble(min_data[2]));
		double[] diff_hsv = Functions.convertRGBtoHSV(
			Double.parseDouble(min_data[0])+Double.parseDouble(diff_data[0]),
			Double.parseDouble(min_data[1])+Double.parseDouble(diff_data[1]),
			Double.parseDouble(min_data[2])+Double.parseDouble(diff_data[2]));
		ranges.get("ColorR").add(min_hsv[0]);
		ranges.get("ColorR").add(diff_hsv[0]);
		mapFunction("ColorR", funct);
		ranges.get("ColorG").add(min_hsv[1]);
		ranges.get("ColorG").add(diff_hsv[1]);
		mapFunction("ColorG", funct);
		ranges.get("ColorB").add(min_hsv[2]);
		ranges.get("ColorB").add(diff_hsv[2]);
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

	public void setFieldProperties(FieldProperties fp){
		fieldProperties = fp;
	}

	public FieldProperties getFieldProperties(){
		return fieldProperties;
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

	public int getLabel(){
		return label;
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

	public double getDefIfNotBound(String field){
		if(keyValueMap.containsKey(field)){
			return keyValueMap.get(field).getDefault();
		}
		return ranges.get(field).get(1);
	}

}
