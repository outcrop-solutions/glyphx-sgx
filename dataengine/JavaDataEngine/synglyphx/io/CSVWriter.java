package synglyphx.io;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.*;
import java.nio.file.Paths;
import synglyphx.glyph.Node;
import synglyphx.glyph.Edge;
import synglyphx.util.ColorIndex;
import synglyphx.util.BaseObject;
import synglyphx.glyph.CoordinateMap;
import synglyphx.link.LinkTemplate;
import synglyphx.util.ErrorHandler;

public class CSVWriter {
	
	private int nodeCount;
	private Map<Integer,Node> allNodes;
	private ArrayList<Edge> edges;
	private HashMap<Integer,String> excelMap;
	private ArrayList<BaseObject> base_objects;
	private ArrayList<String> baseObjectLines;
	private HashMap<Integer, CoordinateMap> rootCoords;
	private Map<Integer, LinkTemplate> link_temps;
	private ArrayList<Integer> excluded;
	private String antzHeader; //94 Columns...
	private String thru10_25;
	private String thru41_50;
	private String thru65_92;
	private String line1;
	private String line2;
	private String line3;
	private String line4;
	private String line5;
	private String line6;
	private String line7;
	private String outDir;
	private String imageDir;
	private String noURLLocation;
	private boolean win;
	private boolean remove_scale_zero;

	public CSVWriter(int nodeCount, Map<Integer,Node> nodes, String outDir,
		String[] colorStr, String app, ArrayList<BaseObject> base_objects,
		HashMap<Integer, CoordinateMap> rootCoords, boolean scale_zero,
		Map<Integer, LinkTemplate> link_temps){
		this.nodeCount = nodeCount;
		this.allNodes = nodes;
		this.base_objects = base_objects;
		this.rootCoords = rootCoords;
		this.remove_scale_zero = scale_zero;
		this.link_temps = link_temps;
		//System.out.println(nodeCount);
		Logger.getInstance().add(outDir);
		if(app.equals("DataMapper")){
			this.outDir = outDir+"/usr/csv/";
			this.imageDir = outDir+"/usr/images/downloadedMap.jpg";
		}else if(app.equals("GlyphViewer")){
			this.outDir = outDir+"/antz/";
			this.imageDir = outDir+"/antz/base_image_2.png";
		}

		String os = System.getProperty("os.name");
        String w = "windows";
        if(os.toLowerCase().contains(w.toLowerCase())){
        	this.win = true;
        }else{
        	this.win = false;
        }
		Logger.getInstance().add(this.outDir);
		setAntz();
		newBegin(app);

		if(app.equals("DataMapper")){
			antzGlobals(colorStr);
			//begin();
		}
	}

	public void begin(){

		createExcelMap();
		try{
			FileWriter epi = new FileWriter(outDir+"ExcelPosIdentifier.csv");  
			BufferedWriter epibfw = new BufferedWriter(epi);

			epibfw.write("record_id,excel_id\n");
			int baseDataPos = 0;
	        for(int i=1; i< nodeCount; i++){
	    		String eposid = "";
		        Node temp = allNodes.get(i);
		        if(temp.getParent() == 0){baseDataPos += 1;}
		        if(temp.getTag() == null){
		        	eposid += String.valueOf(i+7) +",";
		        }else{
		        	eposid += String.valueOf(i+7) +","+getExelPos(temp.getTagPos(),baseDataPos);
		        }
		        eposid += "\n";
		        epibfw.write(eposid);
	        }
	        epibfw.close();
	    }catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }

	}

	private void setAntz(){

		antzHeader = "id,type,data,selected,parent_id,branch_level,child_id,child_index,child_count,";
		antzHeader += "ch_input_id,ch_output_id,ch_last_updated,average,sample,aux_a_x,aux_a_y,aux_a_z,aux_b_x,aux_b_y,";
		antzHeader += "aux_b_z,color_shift,rotate_vec_x,rotate_vec_y,rotate_vec_z,rotate_vec_s,scale_x,scale_y,scale_z,";
		antzHeader += "translate_x,translate_y,translate_z,tag_offset_x,tag_offset_y,tag_offset_z,rotate_rate_x,";
		antzHeader += "rotate_rate_y,rotate_rate_z,rotate_x,rotate_y,rotate_z,scale_rate_x,scale_rate_y,scale_rate_z,";
		antzHeader += "translate_rate_x,translate_rate_y,translate_rate_z,translate_vec_x,translate_vec_y,translate_vec_z,";
		antzHeader += "shader,geometry,line_width,point_size,ratio,color_index,color_r,color_g,color_b,color_a,color_fade,";
		antzHeader += "texture_id,hide,freeze,topo,facet,auto_zoom_x,auto_zoom_y,auto_zoom_z,trigger_hi_x,trigger_hi_y,";
		antzHeader += "trigger_hi_z,trigger_lo_x,trigger_lo_y,trigger_lo_z,set_hi_x,set_hi_y,set_hi_z,set_lo_x,set_lo_y,";
		antzHeader += "set_lo_z,proximity_x,proximity_y,proximity_z,proximity_mode_x,proximity_mode_y,proximity_mode_z,";
		antzHeader += "segments_x,segments_y,segments_z,tag_mode,format_id,table_id,record_id,size\n";

		thru10_25 = "0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,";
		thru41_50 = "0,0,0,0,0,0,0,0,0,0,";
		thru65_92 = "0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,";

		line1 = "1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
		//line2 = "2,1,2,0,0,0,0,2,3,0,0,0,0,1,0,0,0,0,0,0,0,0,-0.143424,-0.826509,-0.544345,1,1,1,1.344855,7.749982,6.27558,0,0,1,0,0,0,57.020073,189.844513,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9.376762,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
		line2 = "2,1,2,0,0,0,0,2,3,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,350,350,0,0,1,0,0,0,45,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,488,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
		line3 = "3,1,3,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,-1,1,1,1,-0.5,0,571.75,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
		line4 = "4,1,4,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,1,1,1,0,-90,7,0,0,1,0,0,0,90,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
		line5 = "5,1,5,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,-1,0,0,1,1,1,85,0,7,0,0,1,0,0,0,90,270,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,0,420\n";
	
	}

	public void newBegin(String app){

		ColorIndex ci = new ColorIndex();
		String nodeName = "";
		String tagName = "";
		if(app.equals("DataMapper")){
			nodeName = "antz0001node.csv";
			tagName = "antz0001tag.csv";
		}else{
			nodeName = "antz.csv";
			tagName = "antztag.csv";
		}

		Logger.getInstance().add(app);

		createBaseObjectStrings(app);
		System.out.println("entering try catch");

		try{
			//FileWriter file = new FileWriter("C:/Users/Bryan/Desktop/Test on Antz/usr/csv/ANTZ0001.csv");  
			FileWriter file = new FileWriter(outDir+nodeName);
			BufferedWriter bf = new BufferedWriter(file);
			//FileWriter f = new FileWriter("C:/Users/Bryan/Desktop/Test on Antz/usr/csv/ANTzTag0001.csv"); 
			FileWriter f = new FileWriter(outDir+tagName);  
			BufferedWriter bfw = new BufferedWriter(f);

			if(app.equals("DataMapper")){
				noURLLocation = outDir+"/../../nourl.html";
			}else{
				Logger.getInstance().add(Paths.get(".").toAbsolutePath().normalize().toString());
				noURLLocation = Paths.get(".").toAbsolutePath().normalize().toString()+"/nourl.html";
			}
		    Logger.getInstance().add(noURLLocation);

		    File nourl = new File(noURLLocation);
		    if(!nourl.exists()){
		       	createNoUrl();
		    }

			bfw.write("id,record_id,table_id,title,description\n");
	        bf.write(antzHeader);
	        bf.write(line1);
	        bf.write(line2);
	        bf.write(line3);
	        bf.write(line4);
	        bf.write(line5);
	        
	        for(int i=0; i < baseObjectLines.size(); i++){
	        	bf.write(baseObjectLines.get(i));
	        }

	        int global_offset = 5+baseObjectLines.size();

	        Logger.getInstance().add(String.valueOf(nodeCount)+" nodes printing...");

	        boolean print = true;
	        String lastRootXYZ = "";
	        int firstRoot = 1;
	        excluded = new ArrayList<Integer>();
	        HashMap<Integer,Double> rotation_lookup = new HashMap<Integer,Double>();
	        int i;
	        for(i = 1; i< nodeCount; i++){
	        	String line = "";
	        	String tag = "";
	        	Node temp = allNodes.get(i);
	        	String xyz = "";
	        	if(rootCoords.get(firstRoot).toMerge()){
	        		xyz = String.valueOf(temp.getX())+String.valueOf(temp.getY())+String.valueOf(temp.getZ());
	        		if(rootCoords.get(firstRoot).containsXYZ(xyz)){
	        			if(rootCoords.get(firstRoot).getFirstRootID(xyz) != i){
	        				rotation_lookup.put(i, temp.getRZ());
	        				print = false;
	        			}else{
	        				lastRootXYZ = xyz;
	        			}
	        		}
	        	}
	        	if(print && isScaleGTZero(temp)){
			        tag += String.valueOf(i-1) +",";
			        tag += String.valueOf(i+global_offset) +",";
			        tag += "0,";
			        tag += "\"<a href=\""+temp.getURL()+"\">"+temp.getTag()+"</a>\",";
			        tag += "\""+temp.getDesc() +"\"";
			        tag += "\n";

			        line += String.valueOf(i+global_offset) +",";//id
			        line += "5,";					//type
			        line += String.valueOf(i+global_offset) +",";//data
			        line += "0,";					//selected
			        if(temp.getParent()==0){
			        	line += String.valueOf(temp.getParent()) +",";//parent_id
			        }else{
			        	if(rotation_lookup.containsKey(temp.getParent()) && rootCoords.get(firstRoot).toMerge()){
			        		line += String.valueOf(rootCoords.get(firstRoot).getFirstRootID(lastRootXYZ)+global_offset) +",";//parent_id
			        	}else{
			        		line += String.valueOf(temp.getParent()+global_offset) +",";//parent_id
			        	}
			        }
			        line += String.valueOf(temp.getBranchLevel() +",");//branch_level
			        line += "0,";					//child_id
			        line += "0,";					//child_index
			        if(rootCoords.get(firstRoot).toMerge() && temp.getParent()==0){
			        	line += String.valueOf(rootCoords.get(firstRoot).getFirstRootChildCount(lastRootXYZ) +",");//child_count
			        }else{
			        	line += String.valueOf(temp.getNumChildren() +",");//child_count
			        }
			        line += thru10_25;				//ch_input_id to rotate_vec_s
			        line += String.valueOf(temp.getSX())+",";//scale_x
			        line += String.valueOf(temp.getSY())+",";//scale_y
			        line += String.valueOf(temp.getSZ())+",";//scale_z
			        if(rootCoords.get(firstRoot).toMerge() && rotation_lookup.containsKey(temp.getParent())){
			        	if(rotation_lookup.get(temp.getParent()) == 0){line += String.valueOf(temp.getX())+",";}//translate_x
			        	else{line += String.valueOf(rotation_lookup.get(temp.getParent()))+",";}//translate_x
			        }else{
			        	line += String.valueOf(temp.getX())+",";//translate_x
			        }
			        line += String.valueOf(temp.getY())+",";//translate_y
			        line += String.valueOf(temp.getZ())+",";//translate_z
			        line += "0,1.5,0,";				//tag_offset_x,y,z
			        line += String.valueOf(temp.getRRX())+",";//rotate_rate_x
			        line += String.valueOf(temp.getRRY())+",";//rotate_rate_y
			        line += String.valueOf(temp.getRRZ())+",";//rotate_rate_z
			        line += String.valueOf(temp.getRX())+",";//rotate_x
			        line += String.valueOf(temp.getRY())+",";//rotate_y
			        line += String.valueOf(temp.getRZ())+",";//rotate_z
			        line += thru41_50;				//scale_rate_x to shader
			        line += String.valueOf(temp.getGeo())+",";//geometry
			        line += "1,0,";					//line_width & point_size
			        line += String.valueOf(temp.getRatio())+",";//ratio
			        line += String.valueOf(ci.getIndex(temp.getCR(),temp.getCG(),temp.getCB()) +",");//color_index
			        line += String.valueOf(temp.getCR())+",";//color_r
			        line += String.valueOf(temp.getCG())+",";//color_g
			        line += String.valueOf(temp.getCB())+",";//color_b
			        line += String.valueOf(temp.getAlpha())+",";//color_a
			        line += "0,0,0,0,";					//color_fade to freeze
			      	line += String.valueOf(temp.getTopo())+",";//topo
			      	line += thru65_92;				//facet to table_id
			      	line += String.valueOf(i+global_offset) +",";//record_id
			      	line += "420";					//size
			        line += "\n";

			        bf.write(line);
			        bfw.write(tag);
			    }else{
			    	excluded.add(i);
			    }
		    	
		    	if(rootCoords.get(firstRoot).getLastID() == i){
		    		firstRoot = rootCoords.get(firstRoot).getLastTemp()+1;
		    	}
		    	print = true;
	        }

	        printLinks(bf, bfw, i, global_offset);

	        bf.close();
	        bfw.close();
	        ErrorHandler.getInstance().csvWriterCompleted();
	    }catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	private int evaluateID(int id) {
		if(excluded.contains(id)){
			return evaluateID(allNodes.get(id).getParent());
		}
		else{
			return id;
		}
	}

	public void printLinks(BufferedWriter bf, BufferedWriter bfw, int index, int offset) throws IOException {

		for(Map.Entry<Integer, LinkTemplate> entry : link_temps.entrySet()){
			ArrayList<Integer> parent_ids = entry.getValue().linkParentID();
			ArrayList<Integer> child_ids = entry.getValue().linkChildID();
			int link_index = 0;
			index += offset-1;
			System.out.println(offset);
			for(int i = 0; i < entry.getValue().linkCount(); i++){
				index++;
				int p_id = evaluateID(parent_ids.get(link_index));
				int c_id = evaluateID(child_ids.get(link_index));
	
				if(p_id != 0 && c_id != 0){
					String out = ""; 
					String key_node_id = String.valueOf(parent_ids.get(link_index)+offset);
					String value_node_id = String.valueOf(child_ids.get(link_index)+offset);
					String color_r = entry.getValue().getCR();
					String color_g = entry.getValue().getCG();
					String color_b = entry.getValue().getCB();
					if(entry.getValue().inheritColor()){ //Capture color from parent
						Node temp = allNodes.get(parent_ids.get(link_index));
						color_r = String.valueOf(temp.getCR());
						color_g = String.valueOf(temp.getCG());
						color_b = String.valueOf(temp.getCB());
					}
					//System.out.println(key_node_id+", "+value_node_id);
					out += index+",7,"+index+",0,"+key_node_id+",1,"+value_node_id+",";
					out += "0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1.5,0,0,0,0,0,0,0,0,0,";
					out += "0,0,0,0,0,0,0,"+entry.getValue().getGeo()+",1,0,0.1,0,";
					out += color_r+","+color_g+","+color_b+","+entry.getValue().getAlpha()+",";
					out += "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,"+index+",420\n";
					bf.write(out);
					bfw.write(entry.getValue().outputTag(link_index, index));
				}
				link_index++;
			}
		}
	}

	private boolean isScaleGTZero(Node temp){

		if(remove_scale_zero){
			if(temp.getSX() == 0 && temp.getSY() == 0 && temp.getSZ() == 0 && temp.getParent() != 0){
				return false;
			}
		}
		return true;
	}

	public void createExcelMap(){
		excelMap = new HashMap<Integer,String>();
		excelMap.put(1,"A");
		excelMap.put(2,"B");
		excelMap.put(3,"C");
		excelMap.put(4,"D");
		excelMap.put(5,"E");
		excelMap.put(6,"F");
		excelMap.put(7,"G");
		excelMap.put(8,"H");
		excelMap.put(9,"I");
		excelMap.put(10,"J");
		excelMap.put(11,"K");
		excelMap.put(12,"L");
		excelMap.put(13,"M");
		excelMap.put(14,"N");
		excelMap.put(15,"O");
		excelMap.put(16,"P");
		excelMap.put(17,"Q");
		excelMap.put(18,"R");
		excelMap.put(19,"S");
		excelMap.put(20,"T");
		excelMap.put(21,"U");
		excelMap.put(22,"V");
		excelMap.put(23,"W");
		excelMap.put(24,"X");
		excelMap.put(25,"Y");
		excelMap.put(26,"Z");
	}

	public String getExelPos(int col, int row){

		int first = col/27;
		int second = col%26;
		if(second == 0){second = 26;}
		String place = "";
		if(first == 0){
			place = excelMap.get(second)+String.valueOf(row+1);
			return place;
		}else{
			place = excelMap.get(first)+excelMap.get(second)+String.valueOf(row+1);
		}

		return place;
	}

	public void antzGlobals(String[] colorStr){

		String cr = String.format("%.6f", Double.parseDouble(colorStr[0]) / 255.0);
		String cg = String.format("%.6f", Double.parseDouble(colorStr[1]) / 255.0);
		String cb = String.format("%.6f",Double.parseDouble(colorStr[2]) / 255.0);

		try{
			FileWriter file = new FileWriter(outDir+"antzglobals.csv");
			BufferedWriter bf = new BufferedWriter(file);

			bf.write("np_globals_id,map_path,item_id,element,type,permisions,name,desc,value\n");
			bf.write("1,\"np_gl\",1,\"alpha_mode\",\"i\",0,\"\",\"\",\"1\"\n");
			bf.write("2,\"np_gl\",1,\"background_rgba\",\"ffff\",0,\"\",\"\",\""+cr+","+cg+","+cb+",1.000000\"\n");
			bf.write("3,\"np_gl\",1,\"fullscreen\",\"i\",0,\"\",\"\",\"1\"\n");
			bf.write("4,\"np_gl\",1,\"window_size_xy\",\"ii\",0,\"\",\"\",\"800, 510\"\n");
			bf.write("5,\"np_gl\",1,\"position_xy\",\"ii\",0,\"\",\"\",\"40, 40\"\n");
			bf.write("6,\"np_gl\",1,\"hud_level\",\"i\",0,\"\",\"\",\"2\"\n");
			bf.write("7,\"np_gl\",1,\"subsample\",\"i\",0,\"\",\"\",\"1\"\n");
			bf.write("8,\"np_mouse\",1,\"tool\",\"i\",0,\"\",\"\",\"4\"\n");
			bf.write("9,\"np_mouse\",1,\"cam_mode\",\"i\",0,\"\",\"\",\"0\"\n");
			bf.write("10,\"np_mouse\",1,\"pick_mode\",\"i\",0,\"\",\"\",\"3\"\n");
			bf.write("11,\"np_db\",1,\"host_ip\",\"s\",0,\"\",\"\",\"127.0.0.1\"\n");
			bf.write("12,\"np_db\",1,\"user\",\"s\",0,\"\",\"\",\"root\"\n");
			bf.write("13,\"np_db\",1,\"password\",\"s\",0,\"\",\"\",\"admin\"\n");
			bf.write("14,\"np_db\",1,\"db_type\",\"s\",0,\"\",\"\",\"mysql\"\n");
			bf.write("15,\"np_osc\",1,\"tx_ip\",\"s\",0,\"\",\"\",\"127.0.0.1\"\n");
			bf.write("16,\"np_osc\",1,\"rx_ip\",\"s\", 0,\"\",\"\",\"127.0.0.1\"\n");
			bf.write("17,\"np_osc\",1,\"tx_port\",\"i\",0,\"\",\"\",\"8000\"\n");
			bf.write("18,\"np_osc\",1,\"rx_port\",\"i\",0,\"\",\"\",\"9000\"\n");
			bf.write("19,\"np_browser\",1,\"url\",\"s\",0,\"\",\"\",\"http://openantz.com/docs/id.html?id=\"\n");
			bf.write("20 \"np_globals\",1,\"item_count\",\"i\",1,\"\",\"\",\"20\"");

			bf.close();
	    }catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	public void createNoUrl(){

		try{
			FileWriter file = new FileWriter(noURLLocation);  
			BufferedWriter nourl = new BufferedWriter(file);
			
			nourl.write("<html>");
			nourl.write("<head>");
			nourl.write("</head>");
			nourl.write("<body>");
			nourl.write("<p><b>No URL is associated with this object.</b></p>");
			nourl.write("</body>");
			nourl.write("</html>");

			nourl.close();
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
	    }
	}

	public void createBaseObjectStrings(String app){

		baseObjectLines = new ArrayList<String>();
		String parentID = "0";
		String branchLevel = "0";
		String childCount = String.valueOf(base_objects.size());
		double[] logo_pos = new double[2];
		boolean world = true;
		int offset = 2;
		File f = new File(imageDir);
		if(f.exists()){world = false;}
		//Logger.getInstance().add(String.valueOf(world));

		for(int i = 0; i < base_objects.size(); i++){
			String template = "";
			BaseObject bo = base_objects.get(i);
			String[] color = bo.getGridColor();
			String number = String.valueOf(i+offset);
			if(i == 0 && (bo.getName().equals("World") || (bo.getType().equals("Downloaded Map") && world))){number = "1";offset = 1;}
			template = String.valueOf(i+6)+",6,"+String.valueOf(i+6)+",1,"+parentID+","+branchLevel+",0,0,"+childCount+",0,0,0,0,1,"+bo.getGLCount(0)+","+bo.getGLCount(1)+",0,0,0,0,0,0,1,0,0,1,1,1,";
			template += bo.getPosition(0)+","+bo.getPosition(1)+","+bo.getPosition(2)+",0,0,0,0,0,0,"+bo.getRotation(0)+","+bo.getRotation(1)+","+bo.getRotation(2)+",0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,18,"+color[0]+","+color[1]+","+color[2]+",255,";
			template += "0,"+number+","+bo.showGridLines()+",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"+bo.getGLSegX()+","+bo.getGLSegY()+",0,0,0,0,0,420\n";
			baseObjectLines.add(template);
			parentID = "6";
			branchLevel = "1";
			childCount = "0";
			if(i == 0){
				logo_pos[0] = bo.getLogoPos()[0];
				logo_pos[1] = bo.getLogoPos()[1];
			}
		}

		if(app.equals("DataMapper")){
			String logo = String.valueOf(base_objects.size()+6)+",6,"+String.valueOf(base_objects.size()+6)+",1,6,1,0,0,0,0,0,0,0,1,24.48,5.4,0,0,0,0,0,0,1,0,0,1,1,1,";
			logo += String.valueOf(logo_pos[0])+","+String.valueOf(logo_pos[1])+",0.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0.1,2,255,0,0,255,";
			logo += "0,"+String.valueOf(base_objects.size()+offset)+",1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,420\n";
			baseObjectLines.add(logo);
		}

		if(app.equals("GlyphViewer")){
			BaseObject bo = base_objects.get(0);
			if(bo.getType().equals("Downloaded Map")){
				try{
					FileWriter file = new FileWriter(outDir+"DownloadedImageSpecs.txt");
					BufferedWriter bf = new BufferedWriter(file);
					bf.write(bo.getUpdateCheckString());
					bf.close();
				}catch(Exception e){
			        try{
			            e.printStackTrace(ErrorHandler.getInstance().addError());
			        }catch(Exception ex){}
			        e.printStackTrace();
			    }
			}
		}
	}
}