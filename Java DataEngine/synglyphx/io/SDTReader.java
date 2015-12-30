package synglyphx.io;

import java.io.File;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.data.SourceDataInfo;
import synglyphx.util.FieldGroup;
import synglyphx.util.GeoID;
import synglyphx.glyph.Mapper;
import synglyphx.util.ConvertHash;
import synglyphx.io.Logger;
import synglyphx.util.BaseObject;
import synglyphx.glyph.CoordinateMap;
import synglyphx.util.BoundingBox;

public class SDTReader {

	private HashMap<Integer, XMLGlyphTemplate> templates = null;
	private ArrayList<ArrayList<String>> inputs = null;
	private Map<String, String> directMap = null;
	private ArrayList<SourceDataInfo> dataPaths = null;
	private ArrayList<Integer> rootIds = null;
	private ArrayList<BaseObject> base_objects = null;
	private HashMap<String,Integer> dataIds = null;
	private HashMap<String,FieldGroup> gNames = null;
	private String[] colorStr = null;
	private Mapper mapping;
	private CSVReader reader;
	private SQLiteReader sqlReader;
	private int rootCount;
	private int count;
	private String outDir;
	private String app;
	private boolean download = false;

	public SDTReader(String sdtPath, String outDir, String application){
		Logger.getInstance().add("Reading SDT at "+sdtPath);
		absorbXML(sdtPath);
		this.outDir = outDir;
		this.app = application;
	}

	public void generateGlyphs(){
		mapping = new Mapper();
		mapping.addNodeTemplates(templates, count);
		mapping.checkRangeXY(download);
		mapping.generateGlyphTrees(dataPaths, rootIds, outDir, colorStr, app, base_objects);
		SQLiteWriter writer = new SQLiteWriter(dataPaths, outDir);
		writer.writeTableIndex();
		writer.writeAllTables();
	}

	public void absorbXML(String sdtPath) {

		templates = new HashMap<Integer, XMLGlyphTemplate>();
		rootIds = new ArrayList<Integer>();
		count = 0;

		try {

			File file = new File(sdtPath);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			Logger.getInstance().add("Parsed SDT into doc.");
			setInputMap(doc);
			getDataPaths(doc);
			checkFieldGroups(doc);
			getBgColor(doc);
			getBaseObjects(doc);

			NodeList start = doc.getElementsByTagName("Glyphs");

			Node glyphs = start.item(0);
			NodeList roots = glyphs.getChildNodes();

			rootCount = 0;
			for (int i = 0; i < roots.getLength(); i++) {

				Node root = roots.item(i);
				if(!root.getNodeName().equals("#text")){
					XMLGlyphTemplate temp = new XMLGlyphTemplate();

					NodeList categories = root.getChildNodes();
					if(rootCount == 0){dataPaths.get(rootCount).setRootID(1);}
					else{
						dataPaths.get(rootCount).setRootID(count+1);
					}
					addNode(categories, temp, 0);
					rootCount++;
				}

			}
		} catch (Exception ex) {
			ex.printStackTrace();
			Logger.getInstance().add("Failed to parse SDT file.");
		}

	}

	private String getValue(String tag, Element element) {

		NodeList nodes = element.getElementsByTagName(tag).item(0).getChildNodes();
		Node node = (Node) nodes.item(0);

		return node.getNodeValue();

	}

	private String getFunction(Element element) {

		Node node = element.getElementsByTagName("Function").item(0);
		Element ele = (Element) node;

		return ele.getAttribute("type");

	}

	private String getInput(Element element) {

		Node node = element.getElementsByTagName("Binding").item(0);
		Element ele = (Element) node;

		return ele.getAttribute("id");

	}

	private ArrayList<String> getInputFields(Element ele) {

		String id = ele.getAttribute("id");
		String table = ele.getAttribute("table");
		String field = ele.getAttribute("field");

		ArrayList<String> array = new ArrayList<String>();
		array.add(id);
		array.add(table);
		array.add(field);

		//Logger.getInstance().add("Retrieved input fields.");
		return array;

	}

	private void handleParent(int pID, Node parent){

		XMLGlyphTemplate p = templates.get(pID);
		int p_branch = p.getBranchLevel();
		NodeList children = parent.getChildNodes();
		for(int i=0; i<children.getLength();i++){

			Node child = children.item(i);
			if(child.getNodeName().equals("Glyph")){
				p.addChild();
				XMLGlyphTemplate childTemp = new XMLGlyphTemplate();
				childTemp.setBranchLevel(p_branch+1);
				NodeList categories = child.getChildNodes();
				addNode(categories, childTemp, pID);

			}
		}

	}

	private void addNode(NodeList categories, XMLGlyphTemplate temp, int pID){

		temp.setChildOf(pID);
		count++;
		templates.put(count,temp);
		if(pID == 0){
			rootIds.add(count);
			temp.setBranchLevel(0);
		}
		dataPaths.get(rootCount).setLastID(count);

		for (int j = 0; j < categories.getLength(); j++) {

			Node category = categories.item(j);
			if(!category.getNodeName().equals("#text") && !category.getNodeName().equals("Children")){
				NodeList values = category.getChildNodes();
								
				for (int k = 0; k < values.getLength(); k++){

					Node value = values.item(k);
					if(!value.getNodeName().equals("#text")){

						if (value.getNodeType() == Node.ELEMENT_NODE) {

							Element element = (Element) value;
							boolean min = true;
							boolean diff = true;
							boolean bind = true;

							try{
								getValue("Min", element);
							}catch(Exception e){
								min = false;
							}

							try{
								getValue("Difference", element);
							}catch(Exception e){
								diff = false;
							}

							try{
								getInput(element);
							}catch(Exception e){
								bind = false;
							}

							if(min && diff && !value.getNodeName().equals("RGB")){
									double min_e = Double.parseDouble(getValue("Min", element));
									temp.addMinMax(category.getNodeName()+value.getNodeName(), min_e, min_e+Double.parseDouble(getValue("Difference", element)));
									temp.mapFunction(category.getNodeName()+value.getNodeName(), getFunction(element));
									if(getFunction(element).contains("To Value")){
										addKeysAndValues(temp, element, getFunction(element), category.getNodeName()+value.getNodeName());
									}
							}
							else if(min && !diff && !value.getNodeName().equals("RGB")){
									temp.addMinMax(category.getNodeName()+value.getNodeName(), Double.parseDouble(getValue("Min", element)), 0.0);
									temp.mapFunction(category.getNodeName()+value.getNodeName(), getFunction(element));
									if(getFunction(element).contains("To Value")){
										addKeysAndValues(temp, element, getFunction(element), category.getNodeName()+value.getNodeName());
									}
							}

					
							if(category.getNodeName().equals("Geometry")){
								Element e = (Element) category;
								temp.setSurface(getValue("Surface", e));
								temp.setRatio(Double.parseDouble(getValue("Ratio", e)));
								if(value.getNodeName().equals("Shape")){
									temp.setShape(getValue("Value", element));
									if(getFunction(element).contains("To Value")){
										addKeysAndValues(temp, element, getFunction(element), category.getNodeName()+value.getNodeName());
										temp.mapFunction(category.getNodeName()+value.getNodeName(), getFunction(element));
									}
								}
							}
							else if(value.getNodeName().equals("Type")){
								temp.setTopo(getValue("Value", element));
								if(getFunction(element).contains("To Value")){
									addKeysAndValues(temp, element, getFunction(element), category.getNodeName()+value.getNodeName());
									temp.mapFunction(category.getNodeName()+value.getNodeName(), getFunction(element));
								}
							}
							else if(category.getNodeName().equals("Tag") && value.getNodeName().equals("Binding")){
								try{
									temp.setTag(directMap.get(element.getAttribute("id")));
									temp.mapInput("Tag", directMap.get(element.getAttribute("id")));
								}catch(Exception e){
									temp.setTag("");
								}
							}
							else if(category.getNodeName().equals("Description") && value.getNodeName().equals("Binding")){
								try{
									temp.setDescription(directMap.get(element.getAttribute("id")));
									temp.mapInput("Description", directMap.get(element.getAttribute("id")));
								}catch(Exception e){
									temp.setDescription("");
								}
							}
							else if(value.getNodeName().equals("RGB")){
								if(diff){
									temp.setRGB(getValue("Min", element),getValue("Difference", element), getFunction(element));
								}else{
									temp.setRGB(getValue("Min", element),"0,0,0", getFunction(element));
								}
								if(getFunction(element).contains("To Value")){
										addKeysAndValues(temp, element, getFunction(element), category.getNodeName()+value.getNodeName());
								}
							}
							else if(category.getNodeName().equals("URL") && value.getNodeName().equals("Binding")){
								try{
									temp.setURL(directMap.get(element.getAttribute("id")));
									temp.mapInput("URL", directMap.get(element.getAttribute("id")));
								}catch(Exception e){
									temp.setURL("");
								}
								//System.out.println(temp.getURL());
							}

							if(bind){
								temp.mapInput(category.getNodeName()+value.getNodeName(), directMap.get(getInput(element)));
								checkForFunctionMinMax(temp, element);
							}
								
						}
					}
				}
			}
			else if(category.getNodeName().equals("Children")){
				handleParent(count, category);
			}
		}
	}

	private void setInputMap(Document doc){

		inputs = new ArrayList<ArrayList<String>>();
		directMap = new HashMap<String, String>();
		ConvertHash convert = new ConvertHash();
		NodeList start = doc.getElementsByTagName("InputFields");

		for(int j=0;j<start.getLength();j++){
			Node field = start.item(j);
			NodeList fields = field.getChildNodes();

			for(int i=0; i < fields.getLength(); i++){
				Node node = fields.item(i);
				if(node.getNodeName().equals("InputField")){
					Element e = (Element) node;
					inputs.add(getInputFields(e));
				}
			}
		}
		for(int i=0; i < inputs.size(); i++){
			String code = convert.getHash(inputs.get(i).get(0), inputs.get(i).get(1), inputs.get(i).get(2));
			directMap.put(code, inputs.get(i).get(2));

		}
		Logger.getInstance().add("Set input map.");
	}

	private void getDataPaths(Document doc){

		dataPaths = new ArrayList<SourceDataInfo>();
		dataIds = new HashMap<String,Integer>();
		NodeList start = doc.getElementsByTagName("Datasources");
		Node source = start.item(0);

		NodeList sources = source.getChildNodes();

		int holder = 0;
		for(int i=0; i < sources.getLength(); i++){
			Node node = sources.item(i);
			if(node.getNodeName().equals("File")){
				Element e = (Element) node;
				if(e.getAttribute("type").equals("CSV")){
					SourceDataInfo csv = new SourceDataInfo();
					csv.setID(e.getAttribute("id"));
					//String[] fileName = getValue("Name", e).split("/");
					//csv.setTable(fileName[fileName.length-1]);
					csv.setTable("OnlyTable");
					csv.setType("csv");
					csv.setPath(getValue("Name", e));
					dataPaths.add(csv);
					dataIds.put(csv.getID()+csv.getTable(),holder);
					holder++;
					System.out.println(csv.getID()+csv.getTable());
					Logger.getInstance().add(csv.getID()+csv.getTable());
				}else if(e.getAttribute("type").equals("SQLITE3")){
					String id = e.getAttribute("id");
					String path = getValue("Name", e);
					NodeList tables = e.getElementsByTagName("Table");
					for(int j=0; j<tables.getLength(); j++){
						NodeList tableNodes = tables.item(j).getChildNodes();
						Node table = (Node) tableNodes.item(0);
						SourceDataInfo tb = new SourceDataInfo();
						tb.setID(id);
						tb.setTable(table.getNodeValue());
						tb.setType("sqlite");
						tb.setPath(path);
						dataPaths.add(tb);
						dataIds.put(tb.getID()+tb.getTable(),holder);
						holder++;
						System.out.println(tb.getID()+tb.getTable());
						Logger.getInstance().add(tb.getID()+tb.getTable());
					}
				}
			}
		}
		Logger.getInstance().add("Retrieved data paths.");
		createDataFrames();
		Logger.getInstance().add("Created DataFrames.");

	}

	void getBgColor(Document doc){

		NodeList start = doc.getElementsByTagName("SceneProperties");
		Node node = start.item(0);
		Element element = (Element) node;
		colorStr = getValue("BackgroundColor", element).split(",");

		Logger.getInstance().add("Retrieved Bg color.");
		
	}

	private void createDataFrames(){

		for(int i=0; i < dataPaths.size();i++){
			if(dataPaths.get(i).getType().equals("csv")){
				reader = new CSVReader();
				reader.createDataFrame(dataPaths.get(i).getPath());
				dataPaths.get(i).setDataFrame(reader.getDataFrame());
			}else if(dataPaths.get(i).getType().equals("sqlite")){
				sqlReader = new SQLiteReader();
				sqlReader.createDataFrame(dataPaths.get(i).getPath(),dataPaths.get(i).getTable());
				dataPaths.get(i).setDataFrame(sqlReader.getDataFrame());
			}
		}

	}

	private void addKeysAndValues(XMLGlyphTemplate temp, Element element, String type, String field){

		String def = "";
		String key = "";
		String value = "";
		Node node = element.getElementsByTagName("Function").item(0);
		NodeList nodelist = node.getChildNodes();
		def = getValue("Default", element);
		GeoID geo = new GeoID();

		for(int i=0; i<nodelist.getLength();i++){
			Node next = nodelist.item(i);

			if(next.getNodeName().equals("KeyValuePairs")){
				Element ele = (Element) next;
				NodeList pairs = next.getChildNodes();

				for(int j=0;j<pairs.getLength();j++){
					Node kvp = pairs.item(j);

					if(kvp.getNodeName().equals("KeyValuePair")){
						Element kv = (Element) kvp;

						key = getValue("Key", kv);
						value = getValue("Value", kv);
						
						String [] defSplit;
						String [] valSplit;
						if(field.equals("ColorRGB")){
							defSplit = def.split(",");
							valSplit = value.split(",");
							temp.addKeyValue("ColorR", type, defSplit[0], key, valSplit[0]);
							temp.addKeyValue("ColorG", type, defSplit[1], key, valSplit[1]);
							temp.addKeyValue("ColorB", type, defSplit[2], key, valSplit[2]);
						}else if(field.equals("GeometryShape")){
							String sf = temp.getSurface();
							temp.addKeyValue(field,type, String.valueOf(geo.getValue(def,sf)), key, String.valueOf(geo.getValue(value,sf)));
					 	}else if(field.equals("VirtualTopologyType")){
							temp.addKeyValue(field,type, String.valueOf(geo.getTopoValue(def)), key, String.valueOf(geo.getTopoValue(value)));
					 	}else{
							temp.addKeyValue(field, type, def, key, value);
						}
					}
				}
			}
		}
	}

	public void checkFieldGroups(Document doc){

		Logger.getInstance().add("Checking field groups..."); 

		ArrayList<ArrayList<String>> input = null;
		NodeList start = doc.getElementsByTagName("FieldGroups");
		gNames = new HashMap<String,FieldGroup>();

		for(int j=0;j<start.getLength();j++){
			Node field = start.item(j);
			NodeList fields = field.getChildNodes();

			for(int i=0; i < fields.getLength(); i++){
				Node node = fields.item(i);
				if(node.getNodeName().equals("FieldGroup")){
					Element e = (Element) node;
					String gName = e.getAttribute("name");
					NodeList groups = node.getChildNodes();
					input = new ArrayList<ArrayList<String>>();
					String id = "";
					String table = "";
					for(int k=0; k<groups.getLength(); k++){
						Node inField = groups.item(k);

						if(inField.getNodeName().equals("InputField")){
							Element f = (Element) inField;
							input.add(getInputFields(f));
							id = f.getAttribute("id");
							table = f.getAttribute("table");
						}
					}

					//System.out.println(id+table);
					gNames.put(gName, new FieldGroup(dataPaths.get(dataIds.get(id+table))));
					gNames.get(gName).setName(gName);
					gNames.get(gName).setColNames(input);
				}
			}
		}

		Logger.getInstance().add("Finished checking field groups.");
	}

	public void checkForFunctionMinMax(XMLGlyphTemplate temp, Element element) {

		try{
			Node node = element.getElementsByTagName("Function").item(0);
			NodeList nodelist = node.getChildNodes();
			Node mm_ = (Element) nodelist.item(1);
			Element mm = (Element) mm_;

			if(mm.getAttribute("type").equals("UserSpecified")){
				Double min = Double.parseDouble(getValue("Min", mm));
				Double diff = Double.parseDouble(getValue("Difference", mm));
				String field = directMap.get(getInput(element));
				temp.addMinMaxField(field,min,diff);
			}
			else if(mm.getAttribute("type").equals("FieldGroup")){
				String gName = getValue("Group", mm);
				String field = directMap.get(getInput(element));
				//System.out.println(field+" | "+String.valueOf(gNames.get(gName).getMin())+" | "+String.valueOf(gNames.get(gName).getMax()));
				temp.addMinMaxField(field,gNames.get(gName).getMin(),gNames.get(gName).getMax());
			}

		}catch(Exception e){
			//e.printStackTrace();
		}

	}

	public void getBaseObjects(Document doc){

		base_objects = new ArrayList<BaseObject>();
		NodeList baseObjects = doc.getElementsByTagName("BaseObjects");

		for(int i=0;i<baseObjects.getLength();i++){
			Element baseObject = (Element) baseObjects.item(i);
			NodeList objects = baseObject.getElementsByTagName("BaseObject");

			for(int j=0;j<objects.getLength();j++){
				Node object = objects.item(j);
				Element element = (Element) object;

				String type = element.getAttribute("type");
				String name = "";
				if(element.hasAttribute("default")){
					name = element.getAttribute("default");
				}else if(element.hasAttribute("filename")){
					name = element.getAttribute("filename");
				}
				Logger.getInstance().add("BaseObjects:");
				Logger.getInstance().add(type+" | "+name);

				Element pos = (Element) element.getElementsByTagName("Position").item(0);
				Element rot = (Element) element.getElementsByTagName("Rotation").item(0);
				Element ws = (Element) element.getElementsByTagName("WorldSize").item(0);
				Element gl = (Element) element.getElementsByTagName("GridLines").item(0);

				BaseObject bObject = new BaseObject(type);
				if(type.equals("Downloaded Map")){
					name = "downloadedMap.jpg";
					download = true;
					bObject.setMapInfo(element.getAttribute("mapsource"),element.getAttribute("maptype"));
					bObject.setImageInfo(element.getAttribute("invert"),element.getAttribute("grayscale"),element.getAttribute("bestfit"),element.getAttribute("margin"));
				}
				bObject.setName(name);
				bObject.setPosition(getValue("X", pos), getValue("Y", pos), getValue("Z", pos));
				bObject.setRotation(getValue("AngleX", rot), getValue("AngleY", rot), getValue("AngleZ", rot));
				bObject.setWorldSize(getValue("Width", ws), getValue("Height", ws));
				if(gl != null){
					bObject.setGridLineCount(gl.getAttribute("horizontal"),gl.getAttribute("vertical"));
					bObject.setGridColor(getValue("Color", gl));
				}
				base_objects.add(bObject);
			}
		}
	}

	public void setDLMapBoundingBox(BoundingBox bb, double[] imageSize, double[] corners){
		for(int i = 0; i < base_objects.size(); i++){
			if(base_objects.get(i).getType().equals("Downloaded Map")){
				base_objects.get(i).setImageSize(imageSize);
				base_objects.get(i).setBoundingBox(bb);
				base_objects.get(i).setCornerString(corners);
			}
		}
	}

	public String[] getBaseImages(){

		String[] baseImages = new String[base_objects.size()];

		for(int i = 0; i < base_objects.size(); i++){
			if(base_objects.get(i).getType().equals("Default")){
				baseImages[i] = base_objects.get(i).getName()+".png";
			}else{
				baseImages[i] = base_objects.get(i).getName();
			}
		}
		return baseImages;
	}

	public BaseObject getDownloadedBaseObject(){
		BaseObject temp = new BaseObject();
		for(int i = 0; i < base_objects.size(); i++){
			if(base_objects.get(i).getType().equals("Downloaded Map")){
				temp = base_objects.get(i);
			}
		}
		return temp;
	}

	public ArrayList<SourceDataInfo> getSourceDataInfo(){
		return dataPaths;
	}
}

