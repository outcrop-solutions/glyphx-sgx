package synglyphx.io;

import java.sql.*;
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
import synglyphx.util.*;
import synglyphx.glyph.Mapper;
import synglyphx.io.Logger;
import synglyphx.glyph.CoordinateMap;
import synglyphx.link.SDTLinkReader;
import synglyphx.jdbc.Table;
import synglyphx.jdbc.BasicTable;
import synglyphx.jdbc.MergedTable;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;

public class SDTReader {

	private HashMap<Integer, XMLGlyphTemplate> templates = null;
	private ArrayList<ArrayList<String>> inputs = null;
	private Map<String, String> directMap = null;
	private ArrayList<SourceDataInfo> dataPaths = null;
	private ArrayList<Integer> rootIds = null;
	private ArrayList<BaseObject> base_objects = null;
	private HashMap<String,Integer> dataIds = null;
	private HashMap<String,FieldGroup> gNames = null;
	private SDTLinkReader linkReader = null;
	private String[] colorStr = null;
	private String tagFieldDefault;
	private String tagValueDefault;
	private boolean scaleZeroDefault;
	private Mapper mapping;
	private CSVReader reader;
	private SQLiteReader sqlReader;
	private int rootCount;
	private int count;
	private String outDir;
	private String app;
	private String timestamp;
	private boolean download;
	private boolean updateNeeded;

	public SDTReader(String sdtPath, String outDir, String application){
		Logger.getInstance().add("Reading SDT at "+sdtPath);
		this.outDir = outDir.replace("\\", File.separator);
		this.app = application;
		this.download = false;
		this.updateNeeded = true;
		initXMLReader(sdtPath);
	}

	public void generateGlyphs(){
		
		//double start = System.currentTimeMillis();
		Thread thread = new Thread(){
    		public void run(){
    			double start = System.currentTimeMillis();
      			SQLiteWriter writer = new SQLiteWriter(dataPaths, outDir, rootIds, templates);
      			writer.writeSDTInfo(timestamp);
      			writer.writeTableIndex();
      			double end = System.currentTimeMillis();
      			Logger.getInstance().addT(String.valueOf((end-start)/1000.00));
    		}
  		};
  		thread.start();

		mapping = new Mapper();
		mapping.addNodeTemplates(templates, count);
		mapping.checkRangeXY(download);
		mapping.setDefaults(tagFieldDefault, tagValueDefault, scaleZeroDefault);
		mapping.setLinkTemplates(linkReader.getLinkTemps());
		mapping.generateGlyphTrees(dataPaths, rootIds, outDir, colorStr, app, base_objects);
		try{
			thread.join();
		}catch(InterruptedException ie){
	        ie.printStackTrace();
		}
		double end = System.currentTimeMillis();
		//System.out.print("Time to generate cache: ");
		//System.out.println((end-start)/1000.00);
	}

	public boolean isUpdateNeeded() {
		return updateNeeded;
	}

	public void initXMLReader(String sdtPath) {

		try{
			File file = new File(sdtPath);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			Node transform = doc.getElementsByTagName("Transform").item(0);
			Element tf = (Element) transform;
			timestamp = tf.getAttribute("Timestamp");
			getDataPaths(doc);
			getBaseObjects(doc);
			System.out.print("Datasource count: ");
			System.out.println(dataPaths.size());

			if(!timestamp.equals("") && app.equals("GlyphViewer")){
				updateNeeded = SQLiteReader.isAntzUpdateNeeded(timestamp, outDir, dataPaths);
			}

			if(updateNeeded){
				Logger.getInstance().add("Absorbing XML...");
				absorbXML(doc);
				Logger.getInstance().add("Creating SDTLinkReader...");
				linkReader = new SDTLinkReader(doc, templates, dataPaths, directMap);
				System.out.println("Link Reader created");
			}

		}catch(Exception e){
			//e.printStackTrace();
			try{
			    e.printStackTrace(Logger.getInstance().addError());
			}catch(Exception ex){}
		}

	}

	public void absorbXML(Document doc) {

		templates = new HashMap<Integer, XMLGlyphTemplate>();
		rootIds = new ArrayList<Integer>();
		count = 0;

		try {

			Logger.getInstance().add("Parsed SDT into doc.");
			setInputMap(doc);
			Logger.getInstance().add("Setup input map...");
			checkFieldGroups(doc);
			Logger.getInstance().add("Checking for field groups...");
			getDefaultsAndProperties(doc);
			Logger.getInstance().add("Set defaults and properties...");

			NodeList start = doc.getElementsByTagName("Glyphs");

			Node glyphs = start.item(0);
			NodeList roots = glyphs.getChildNodes();

			System.out.println(roots.getLength());
			rootCount = 0;
			for (int i = 0; i < roots.getLength(); i++) {
				Node root = roots.item(i);
				if(root.getNodeName().equals("Glyph")){
					XMLGlyphTemplate temp = new XMLGlyphTemplate();
					NodeList categories = root.getChildNodes();
					temp.setToMerge(getMergeStatus(root));
					addNode(categories, temp, 0);
					rootCount++;
				}

			}
		} catch (Exception ex) {
			ex.printStackTrace();
			Logger.getInstance().add("Failed to parse SDT file.");
		}
		createDataFrames();
		Logger.getInstance().add("Created DataFrames.");
		setRootAndLastIDs();
		Logger.getInstance().add("Finished absorbing XML.");
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

	private boolean getMergeStatus(Node root){
		Element element = (Element) root;
		if(element.hasAttribute("merge")){
			String to_merge = element.getAttribute("merge");
			if(to_merge.equals("true")){
				return true;
			}
		}
		return false;
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

							if(element.getElementsByTagName("Min").getLength() == 0){
								min = false;
							}

							if(element.getElementsByTagName("Difference").getLength() == 0){
								diff = false;
							}
		
							if(element.getElementsByTagName("Binding").getLength() == 0){
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
									double m_e = Double.parseDouble(getValue("Min", element));
									temp.addMinMax(category.getNodeName()+value.getNodeName(), m_e, m_e);
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
								String field_name = category.getNodeName()+value.getNodeName();
								String code = getInput(element);
								if(field_name.equals("PositionX")){
									temp.setDataSource(getDataPathForTemplate(directMap.get(code), code));
								}
								if(getFunction(element).equals("Text Interpolation")){
									dataPaths.get(temp.getDataSource()).setHasTextInterpolation();
									dataPaths.get(temp.getDataSource()).addTextInterpolationField(directMap.get(code));
								}
								temp.mapInput(field_name, directMap.get(code));
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
			System.out.println(start.getLength());
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
			if(node.getNodeName().equals("Datasource")){
				Element e = (Element) node;
				if(e.getAttribute("type").equals("CSV")){
					SourceDataInfo csv = new SourceDataInfo();
					csv.setID(e.getAttribute("id"));
					csv.setTable("OnlyTable");
					csv.setType("csv");
					csv.setPath(getValue("Name", e));
					dataPaths.add(csv);
					dataIds.put(csv.getID()+csv.getTable(),holder);
					holder++;
					System.out.println(csv.getID()+":"+csv.getTable());
					Logger.getInstance().add(csv.getID()+csv.getTable());
				}else{
					String id = e.getAttribute("id");
					String host = getValue("Host", e);
					String user = "";
					String pass = "";
					try{
						user = getValue("Username", e);
						pass = getValue("Password", e);
					}catch(Exception ex){}
					NodeList tbls = e.getElementsByTagName("Tables");
					Element tblObjects = (Element) tbls.item(0);
					NodeList tables = tblObjects.getElementsByTagName("Table");
					//System.out.println(tables.getLength());
					for(int j=0; j<tables.getLength(); j++){
	 					Node table = tables.item(j); 
	 					Element te = (Element) table;

						SourceDataInfo tb = new SourceDataInfo();
						tb.setID(id);
						tb.setTable(table.getTextContent());
						tb.setType(e.getAttribute("type").toLowerCase());
						if(te.hasAttribute("query")){
							if(!te.getAttribute("query").equals("")){
								tb.setQuery(te.getAttribute("query"));
							}
						}
						tb.setPath(host);
						tb.setHost("jdbc:"+host);
						tb.setUsername(user);
						tb.setPassword(pass);
						dataPaths.add(tb);
						dataIds.put(tb.getID()+tb.getTable(),holder);
						holder++;
						Logger.getInstance().add(tb.getID()+tb.getTable());
					}
				}
			}
		}
		Logger.getInstance().add("Retrieved data paths.");

	}

	void getDefaultsAndProperties(Document doc){

		NodeList defaults = doc.getElementsByTagName("Defaults");
		Node def = defaults.item(0);
		Element def_ele = (Element) def;
		tagFieldDefault = getValue("TagFieldDefault", def_ele).replace(" ", "");
		tagValueDefault = getValue("TagValueDefault", def_ele);
		scaleZeroDefault = false;
		try{
			Element scale = (Element) doc.getElementsByTagName("ScaleZeroDefault").item(0);
			if(scale.getAttribute("remove").equals("true")){
				scaleZeroDefault = true;
			}
		}catch(Exception e){
			Logger.getInstance().add("ScaleZeroDefault tag is missing...");
		}

		Logger.getInstance().add("Retrieved Defaults.");

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
			}else{
				//dataframe creator for JDBC
				Table table = null;
				try{
			    	Driver driver = DriverSelector.getDriver(dataPaths.get(i).getType());
		            Class.forName(driver.packageName());
			        Logger.getInstance().add("Connecting to Server...");

			        driver.createConnection(dataPaths.get(i).getHost(),dataPaths.get(i).getUsername(),dataPaths.get(i).getPassword());
			        //System.out.println(sourceData.getQuery());
			        if(dataPaths.get(i).isMerged()){
			        	table = new MergedTable(dataPaths.get(i).getQuery(), driver);
			        }else{
			        	table = new BasicTable(dataPaths.get(i).getTable(), dataPaths.get(i).getQuery(), driver);
			        }
			    }catch(SQLException se){
			        try{
			            se.printStackTrace(Logger.getInstance().addError());
			        }catch(Exception ex){}
			    }catch(Exception e){
			        try{
			            e.printStackTrace(Logger.getInstance().addError());
			        }catch(Exception ex){}
			    }
			    dataPaths.get(i).setDataFrame(table.createDataFrame());
			}

			if(dataPaths.get(i).hasTextInterpolation()){
				dataPaths.get(i).setTextInterpolationFields();
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
		temp.defaultSetter(field, type, def);
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
						
						String [] valSplit;
						if(field.equals("ColorRGB")){
							valSplit = value.split(",");
							double[] hsv = Functions.convertRGBtoHSV(Double.parseDouble(valSplit[0]),Double.parseDouble(valSplit[1]),Double.parseDouble(valSplit[2]));
							temp.addKeyValue("ColorR", key, String.valueOf(hsv[0]));
							temp.addKeyValue("ColorG", key, String.valueOf(hsv[1]));
							temp.addKeyValue("ColorB", key, String.valueOf(hsv[2]));
						}else if(field.equals("GeometryShape")){
							String sf = temp.getSurface();
							temp.addKeyValue(field, key, String.valueOf(geo.getValue(value,sf)));
					 	}else if(field.equals("VirtualTopologyType")){
							temp.addKeyValue(field, key, String.valueOf(geo.getTopoValue(value)));
					 	}else{
							temp.addKeyValue(field, key, value);
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
				Logger.getInstance().add("BaseObjects:");
				Logger.getInstance().add(type+" | "+name);
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

	public int getDataPathForTemplate(String field, String code){

		ConvertHash convert = new ConvertHash();
		for(int i = 0; i < dataPaths.size(); i++){
			SourceDataInfo tb = dataPaths.get(i);
			if(convert.getHash(tb.getID(),tb.getTable(),field).equals(code)){
				return i;
			}
		}
		return rootCount;
	}

	public void setRootAndLastIDs(){

		try{
			int currentRoot = 1;
			SourceDataInfo currentDataSource = null;
			ArrayList<Integer> usedDS = new ArrayList<Integer>();
			XMLGlyphTemplate prevTemp = null;
			for(int i = 1; i < count+1; i++){
				XMLGlyphTemplate temp = templates.get(i);
				if(temp.getChildOf() == 0){
					if(i != 1){
						currentDataSource.setLastID(i-1); //OLD
						prevTemp.setLastChildID(i-1); //NEW
					}
					if(!usedDS.contains(temp.getDataSource())){
						currentDataSource = dataPaths.get(temp.getDataSource());
					}else{
						dataPaths.add(dataPaths.get(temp.getDataSource()));
						temp.setDataSource(dataPaths.size()-1);
						currentDataSource = dataPaths.get(temp.getDataSource());
					}
					currentDataSource.setRootID(i);
					usedDS.add(temp.getDataSource());
					prevTemp = temp;
				}
				if(i == count){
					currentDataSource.setLastID(i); //OLD
					prevTemp.setLastChildID(i); //NEW
				}
			}
		}catch(Exception e){
			try{
			    e.printStackTrace(Logger.getInstance().addError());
			}catch(Exception ex){}
		}
	}
}

