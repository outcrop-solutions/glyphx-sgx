package synglyphx.glyph;

import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.glyph.GlyphTreeRepository;
import synglyphx.glyph.Node;
import synglyphx.util.GeoID;
import synglyphx.util.Functions;
import synglyphx.data.*;
import synglyphx.io.Logger;
import synglyphx.util.BaseObject;
import synglyphx.link.LinkTemplate;
import synglyphx.link.LinkCreator;

public class GlyphCreator {

	private Map<Integer, XMLGlyphTemplate> temps = null;
	private Map<Integer, LinkTemplate> link_temps = null;
	private ArrayList<SourceDataInfo> csvData = null;
	private ArrayList<Integer> rootIds = null;
	private GlyphTreeRepository glyphRepo = null;
	private HashMap<String,ArrayList<Integer>> toMerge = null;
	private int mappingCount;
	private boolean inner = false;
	private static GeoID geoId = null;
	private int currData;
	private HashMap<Integer, CoordinateMap> rootCoords = null;
	private boolean download;
	private String default_tag_field;
	private String default_tag_value;
	private boolean remove_scale_zero;

	public GlyphCreator(ArrayList<SourceDataInfo> csvData, Map<Integer, XMLGlyphTemplate> temps, int mappingCount, ArrayList<Integer> rootIds){
		this.csvData = csvData;
		this.temps = temps;
		this.rootIds = rootIds;
		this.rootCoords = new HashMap<Integer, CoordinateMap>();
		this.glyphRepo = new GlyphTreeRepository();
		this.mappingCount = mappingCount;
		geoId = new GeoID();
	}

	public void setOutDir(String outDir, String app){
		glyphRepo.setOutDir(outDir, app);
	}

	public void checkRangeXY(boolean dl){
		download = dl;
	}

	public void setDefaults(String tag_field, String tag_value, boolean scale_zero){
		default_tag_field = tag_field;
		default_tag_value = tag_value;
		remove_scale_zero = scale_zero;
	}

	public void setLinkTemplates(Map<Integer, LinkTemplate> link_temps){
		this.link_temps = link_temps;
	}

	public void begin(){

		Thread thread = new Thread(){
			public void run(){
				LinkCreator linkCreator = new LinkCreator(link_temps, temps, csvData, rootIds);
				linkCreator.begin();
			}
		};
		thread.start();

		for(int i=0;i < rootIds.size();i++){
			currData = temps.get(rootIds.get(i)).getDataSource();
			int lastChildID = temps.get(rootIds.get(i)).getLastChildID();
			rootCoords.put(rootIds.get(i), new CoordinateMap(temps.get(rootIds.get(i)).getToMerge(), lastChildID));
			//if(csvData.get(currData).getType().equals("csv") || csvData.get(currData).getType().equals("sqlite3")){
				Query query = new Query(csvData.get(currData).getDataFrame()).all(); 
				Logger.getInstance().add("Executed initial query...");
				Cursor cursor = csvData.get(currData).getDataFrame().query(query);
				Logger.getInstance().add("Returned cursor with size " + String.valueOf(cursor.size()) +"...");

				Logger.getInstance().add(String.valueOf(rootIds.get(i)));
				Logger.getInstance().add(String.valueOf(lastChildID));

				while(cursor.next()){
					for (int j = 1; j < mappingCount+1; j++){
						if(j >= rootIds.get(i) && j <= lastChildID){
							addNode(j, cursor);
						}
					}
				}
			/*}else{
				JDBCNodeGenerator jdbc = new JDBCNodeGenerator(csvData.get(currData));
				jdbc.initialize(temps, rootCoords, glyphRepo, mappingCount);
				jdbc.setDefaults(default_tag_field, default_tag_value);
				jdbc.checkRangeXY(download);
				jdbc.generateNodes();
				glyphRepo = jdbc.returnUpdatedGlyphRepo();
				rootCoords = jdbc.returnUpdatedRootCoords();
			}*/
			rootCoords.get(rootIds.get(i)).setLastID(glyphRepo.getNodeCount()-1);
		}
		try{
			thread.join();
		}catch(InterruptedException ie){
	        ie.printStackTrace();
		}
	}

	public void addNode(int index, Cursor cursor){

		XMLGlyphTemplate nodeTemp = temps.get(index);
		HashMap<String, ArrayList<Double>> ranges = nodeTemp.getRanges();
		Map<String, String> input = nodeTemp.getInputMapping();
		Map<String, String> functions = nodeTemp.getFunctionMapping();

		HashMap<String, Double> setValues = new HashMap<String,Double>();

		ArrayList<String> fieldNames = new ArrayList<String>();
				
		for (Map.Entry<String, String> entry : input.entrySet()) {
			if(!entry.getKey().equals("Tag") && !entry.getKey().equals("Description") && !entry.getKey().equals("URL")){
				fieldNames.add(entry.getKey());
			}
		}

		for(int i=0; i < fieldNames.size(); i++){
			if(functions.get(fieldNames.get(i)).equals("Linear Interpolation") || functions.get(fieldNames.get(i)).equals("Logarithmic Interpolation")){
				double y1;
				double y3;
				if(download && nodeTemp.getChildOf() == 0 && (fieldNames.get(i).equals("PositionX") || fieldNames.get(i).equals("PositionY"))){
					y1 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(0));
					y3 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(1));
				}else{
					y1 = ranges.get(fieldNames.get(i)).get(0);
					y3 = ranges.get(fieldNames.get(i)).get(1);
				}
				double x1;
				double x3;
				if(nodeTemp.minMaxFieldExists(input.get(fieldNames.get(i)))){
					x1 = nodeTemp.getMinMaxField(input.get(fieldNames.get(i))).get(0);
					x3 = nodeTemp.getMinMaxField(input.get(fieldNames.get(i))).get(1);
				}else{
					x1 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(0));
					x3 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(1));
				}
				double x2 = Double.parseDouble(cursor.get(input.get(fieldNames.get(i)))); //returns exact value in this row
				if(x3 > x1){
					if(x3 < Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(1))){
						x3 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(1));
						nodeTemp.updateMinMaxField(input.get(fieldNames.get(i)), x1, x3);
					}
					if(x1 > Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(0))){
						x1 = Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(0));
						nodeTemp.updateMinMaxField(input.get(fieldNames.get(i)), x1, x3);
					}
				}
				if(functions.get(fieldNames.get(i)).equals("Linear Interpolation")){
					setValues.put(fieldNames.get(i), Functions.linearInterpolation(x1,x3,y1,y3,x2));
				}else if(functions.get(fieldNames.get(i)).equals("Logarithmic Interpolation")){
					setValues.put(fieldNames.get(i), Functions.logarithmicInterpolation(x1,x3,y1,y3,x2));
				}
			}
			else if(functions.get(fieldNames.get(i)).equals("Text Interpolation")){
				double x1 = 0;
				double x3 = csvData.get(currData).getDataFrame().getTIMax(input.get(fieldNames.get(i)));
				double y1 = ranges.get(fieldNames.get(i)).get(0);
				double y3 = ranges.get(fieldNames.get(i)).get(1);
				double x2 = csvData.get(currData).getDataFrame().getTIPos(input.get(fieldNames.get(i)),cursor.get(input.get(fieldNames.get(i))));
				setValues.put(fieldNames.get(i), Functions.linearInterpolation(x1,x3,y1,y3,x2));
			}
			else if(functions.get(fieldNames.get(i)).equals("Numeric Field To Value")){
				setValues.put(fieldNames.get(i), Functions.numericToValue(Double.parseDouble(cursor.get(input.get(fieldNames.get(i)))),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("Text Field To Value")){
				setValues.put(fieldNames.get(i), Functions.textToValue(cursor.get(input.get(fieldNames.get(i))),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("Range To Value")){
				setValues.put(fieldNames.get(i), Functions.rangeToValue(Double.parseDouble(cursor.get(input.get(fieldNames.get(i)))),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("None")){
				setValues.put(fieldNames.get(i), Double.parseDouble(csvData.get(currData).getDataFrame().getMinMaxTable().get(input.get(fieldNames.get(i))).get(1)));
			}
		}

		Node node = setFields(fieldNames, setValues, new Node(), nodeTemp, input);

		//TAG STUFF
		node.setDefaultTagValue(default_tag_value);
		if(input.get("Tag") != null){
			//System.out.println(input.get("Tag"));
			node.setTag(input.get("Tag")+": "+cursor.get(input.get("Tag")));
			node.setTagPos(csvData.get(currData).getDataFrame().getHeaderPlace(input.get("Tag")));
		}else if(input.get("Tag") == null && input.get(default_tag_field) != null){
			node.setTag(input.get(default_tag_field)+": "+cursor.get(input.get(default_tag_field)));
			node.setTagPos(csvData.get(currData).getDataFrame().getHeaderPlace(input.get(default_tag_field)));
		}
		//END TAG
		if(input.get("Description") != null){
			node.setDesc(cursor.get(input.get("Description")));
		}
		if(input.get("URL") != null){
			node.setURL(cursor.get(input.get("URL")));
		}

		if(nodeTemp.getChildOf() == 0){
			node.setChildOf(0);
			node.setTemplate(index);
		}else{
			int p = glyphRepo.getNodeCount()-(index - nodeTemp.getChildOf());
			node.setChildOf(p);
			glyphRepo.addEdge(p);
		}
		node.setLineNum(cursor.getCurrentIndex()+1);
		node.setNumChildren(nodeTemp.getNumChildren());
		glyphRepo.addNode(node);

	}

	public static Node setFields(ArrayList<String> fieldNames, HashMap<String, Double> setValues, Node node, XMLGlyphTemplate childTemp, Map<String, String> input){

		//SET POSITION
		if(!fieldNames.contains("PositionX")){
			node.setX(childTemp.getDefIfNotBound("PositionX"));
		}else{
			node.setX(setValues.get("PositionX"));
		}
		if(!fieldNames.contains("PositionY")){
			node.setY(childTemp.getDefIfNotBound("PositionY"));
		}else{
			node.setY(setValues.get("PositionY"));
		}
		if(!fieldNames.contains("PositionZ")){
			node.setZ(childTemp.getDefIfNotBound("PositionZ"));
		}else{
			node.setZ(setValues.get("PositionZ"));
		}

		//SET SCALE
		if(!fieldNames.contains("ScaleX")){
			node.setSX(childTemp.getDefIfNotBound("ScaleX"));
		}else{
			node.setSX(setValues.get("ScaleX"));
		}
		if(!fieldNames.contains("ScaleY")){
			node.setSY(childTemp.getDefIfNotBound("ScaleY"));
		}else{
			node.setSY(setValues.get("ScaleY"));
		}
		if(!fieldNames.contains("ScaleZ")){
			node.setSZ(childTemp.getDefIfNotBound("ScaleZ"));
		}else{
			node.setSZ(setValues.get("ScaleZ"));
		}

		//SET ROTATION
		if(!fieldNames.contains("RotationX")){
			node.setRX(childTemp.getDefIfNotBound("RotationX"));
		}else{
			node.setRX(setValues.get("RotationX"));
		}
		if(!fieldNames.contains("RotationY")){
			node.setRY(childTemp.getDefIfNotBound("RotationY"));
		}else{
			node.setRY(setValues.get("RotationY"));
		}
		if(!fieldNames.contains("RotationZ")){
			node.setRZ(childTemp.getDefIfNotBound("RotationZ"));
		}else{
			node.setRZ(setValues.get("RotationZ"));
		}

		//SET ROTATION RATE
		if(!fieldNames.contains("RotationRateX")){
			node.setRRX(childTemp.getDefIfNotBound("RotationRateX"));
		}else{
			node.setRRX(setValues.get("RotationRateX"));
		}
		if(!fieldNames.contains("RotationRateY")){
			node.setRRY(childTemp.getDefIfNotBound("RotationRateY"));
		}else{
			node.setRRY(setValues.get("RotationRateY"));
		}
		if(!fieldNames.contains("RotationRateZ")){
			node.setRRZ(childTemp.getDefIfNotBound("RotationRateZ"));
		}else{
			node.setRRZ(setValues.get("RotationRateZ"));
		}

		//SET COLOR
		if(!fieldNames.contains("ColorR")){
			node.setColor(childTemp.getDefIfNotBound("ColorR"),childTemp.getDefIfNotBound("ColorG"),childTemp.getDefIfNotBound("ColorB"));
		}else{
			node.setColor(setValues.get("ColorR"),setValues.get("ColorG"),setValues.get("ColorB"));
		}

		//SET ALPHA
		if(!fieldNames.contains("ColorTransparency")){
			node.setAlpha((int)Math.round(childTemp.getDefIfNotBound("ColorTransparency")));
		}else{
			node.setAlpha((int)Math.round(setValues.get("ColorTransparency")));
		}

		if(!fieldNames.contains("GeometryShape")){
			node.setGeometry(geoId.getValue(childTemp.getShape(),childTemp.getSurface()));
		}else{
			node.setGeometry((int)Math.round(setValues.get("GeometryShape")));
		}	

		if(!fieldNames.contains("VirtualTopologyType")){
			node.setTopo(geoId.getTopoValue(childTemp.getTopo()));
		}else{
			node.setTopo((int)Math.round(setValues.get("VirtualTopologyType")));
		}	
		//No associated functions
		node.setSurface(childTemp.getSurface());
		node.setRatio(childTemp.getRatio());
		node.setBranchLevel(childTemp.getBranchLevel());
		node.setNumChildren(childTemp.getNumChildren());

		return node;
		
	}

	public void printGlyphRepo(String[] colorStr, ArrayList<BaseObject> base_objects){
		Logger.getInstance().add("Writing all files...");
		glyphRepo.writeAll(colorStr, base_objects, rootCoords, remove_scale_zero, link_temps);
	}

}