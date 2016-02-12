package synglyphx.glyph;

import java.sql.*;
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
import synglyphx.jdbc.Table;
import synglyphx.jdbc.BasicTable;
import synglyphx.jdbc.MergedTable;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;

public class JDBCNodeGenerator {

	private SourceDataInfo sourceData;
	private Map<Integer, XMLGlyphTemplate> templates;
	private HashMap<Integer, CoordinateMap> rootCoordinates;
	private GlyphTreeRepository glyphRepository;
	private String default_tag_field;
	private String default_tag_value;
	private boolean download;
	private int tempCount;
	private Table table;

	public JDBCNodeGenerator(SourceDataInfo source){
		sourceData = source;
	}

	public void initialize(Map<Integer, XMLGlyphTemplate> temps, HashMap<Integer, CoordinateMap> rootCoords, GlyphTreeRepository glyphRepo, int mappingCount){
		templates = temps;
		rootCoordinates = rootCoords;
		glyphRepository = glyphRepo;
		tempCount = mappingCount;
	}

	public void setDefaults(String field, String value){
		default_tag_field = field;
		default_tag_value = value;
	}

	public void checkRangeXY(boolean dl){
		download = dl;
	}

	public void generateNodes(){

	    try{

	    	Driver driver = DriverSelector.getDriver(sourceData.getType());
            Class.forName(driver.packageName());
	        Logger.getInstance().add("Connecting to Server...");

	        driver.createConnection(sourceData.getHost(),sourceData.getUsername(),sourceData.getPassword());

	        System.out.println(sourceData.getQuery());
	        if(sourceData.isMerged()){
	        	table = new MergedTable(sourceData.getQuery(), driver);
	        }else{
	        	table = new BasicTable(sourceData.getTable(), driver);
	        }

	        String sql = sourceData.getQuery();
			PreparedStatement pstmt = driver.getConnection().prepareStatement(sql);
			Logger.getInstance().add("Executing query for table, "+sourceData.getTable()+", data...");
            ResultSet rs = pstmt.executeQuery();

            while(rs.next()){
            	for (int j = 1; j < tempCount+1; j++){
					if(j >= sourceData.getRootID() && j <= sourceData.getLastID()){
						addNode(j, rs);
					}
				}
            }

	        rs.close();
	        driver.getConnection().close();
	    }catch(SQLException se){
	        try{
	            se.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }catch(Exception e){
	        try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	    }
	}

	public void addNode(int index, ResultSet rs) throws SQLException{

		XMLGlyphTemplate nodeTemp = templates.get(index);
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
				if(download && ((index == 1 && fieldNames.get(i).equals("PositionX"))||(index == 1 && fieldNames.get(i).equals("PositionY")))){
					y1 = Double.parseDouble(table.getMinMaxTable().get(input.get(fieldNames.get(i))).get(0));
					y3 = Double.parseDouble(table.getMinMaxTable().get(input.get(fieldNames.get(i))).get(1));
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
					x1 = Double.parseDouble(table.getMinMaxTable().get(input.get(fieldNames.get(i))).get(0));
					x3 = Double.parseDouble(table.getMinMaxTable().get(input.get(fieldNames.get(i))).get(1));
				}
				double x2 = rs.getDouble(input.get(fieldNames.get(i)).replace("`","")); //returns exact value in this row
				if(functions.get(fieldNames.get(i)).equals("Linear Interpolation")){
					setValues.put(fieldNames.get(i), Functions.linearInterpolation(x1,x3,y1,y3,x2));
				}else if(functions.get(fieldNames.get(i)).equals("Logarithmic Interpolation")){
					setValues.put(fieldNames.get(i), Functions.logarithmicInterpolation(x1,x3,y1,y3,x2));
				}
			}
			else if(functions.get(fieldNames.get(i)).equals("Numeric Field To Value")){
				setValues.put(fieldNames.get(i), Functions.numericToValue(rs.getDouble(input.get(fieldNames.get(i)).replace("`","")),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("Text Field To Value")){
				setValues.put(fieldNames.get(i), Functions.textToValue(rs.getString(input.get(fieldNames.get(i)).replace("`","")),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("Range To Value")){
				setValues.put(fieldNames.get(i), Functions.rangeToValue(rs.getDouble(input.get(fieldNames.get(i)).replace("`","")),nodeTemp.getKeyValueMap().get(fieldNames.get(i))));
			}
			else if(functions.get(fieldNames.get(i)).equals("None")){
				setValues.put(fieldNames.get(i), Double.parseDouble(table.getMinMaxTable().get(input.get(fieldNames.get(i))).get(1)));
			}
		}

		Node node = new Node();
		node = GlyphCreator.setFields(fieldNames, ranges, setValues, node, nodeTemp, input);

		//TAG STUFF
		node.setDefaultTagValue(default_tag_value);
		if(input.get("Tag") != null){
			node.setTag(input.get("Tag")+": "+rs.getString(input.get("Tag").replace("`","")));
			//node.setTagPos(csvData.get(currData).getDataFrame().getHeaderPlace(input.get("Tag")));
		}else if(input.get("Tag") == null && input.get(default_tag_field) != null){
			node.setTag(input.get(default_tag_field)+": "+rs.getString(input.get(default_tag_field).replace("`","")));
			//node.setTagPos(csvData.get(currData).getDataFrame().getHeaderPlace(input.get(default_tag_field)));
		}
		//END TAG
		if(input.get("Description") != null){
			node.setDesc(rs.getString(input.get("Description").replace("`","")));
		}
		if(input.get("URL") != null){
			node.setURL(rs.getString(input.get("URL")).replace("`",""));
		}

		if(nodeTemp.getChildOf() == 0){
			node.setChildOf(0);
			rootCoordinates.get(index).addCoordinate(node,glyphRepository.getNodeCount());
		}else{
			int p = glyphRepository.getNodeCount()-(index - nodeTemp.getChildOf());
			node.setChildOf(p);
			glyphRepository.addEdge(p);
		}
		//node.setLineNum(cursor.getCurrentIndex()+1);
		node.setNumChildren(nodeTemp.getNumChildren());
		glyphRepository.addNode(node);
	}

	public GlyphTreeRepository returnUpdatedGlyphRepo(){
		return glyphRepository;
	}

	public HashMap<Integer, CoordinateMap> returnUpdatedRootCoords(){
		return rootCoordinates;
	}
}