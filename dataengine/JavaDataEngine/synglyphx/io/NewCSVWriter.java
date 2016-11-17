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
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class NewCSVWriter {
	
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

	public NewCSVWriter(int nodeCount, Map<Integer,Node> nodes, String outDir,
		String[] colorStr, String app, ArrayList<BaseObject> base_objects,
		HashMap<Integer, CoordinateMap> rootCoords, boolean scale_zero,
		Map<Integer, LinkTemplate> link_temps){
		this.nodeCount = nodeCount;
		this.allNodes = nodes;
		this.base_objects = base_objects;
		this.rootCoords = rootCoords;
		this.remove_scale_zero = scale_zero;
		this.link_temps = link_temps;

		Logger.getInstance().add(outDir);
		this.outDir = outDir+"/antz/";
		this.imageDir = outDir+"/antz/base_image_2.png";

		String os = System.getProperty("os.name");
        String w = "windows";
        if(os.toLowerCase().contains(w.toLowerCase())){
        	this.win = true;
        }else{
        	this.win = false;
        }

		Logger.getInstance().add(this.outDir);
		newBegin(app);

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

	public void newBegin(String app){

		ColorIndex ci = new ColorIndex();

		System.out.println("entering try catch");

		try{
			FileOutputStream oFile = new FileOutputStream(outDir+"glyphs.sgc", false); 
			BufferedOutputStream bufout = new BufferedOutputStream(oFile);
			DataOutputStream data = new DataOutputStream(bufout);
			data.writeInt(0xa042bc3f);	// magic number

			Logger.getInstance().add(Paths.get(".").toAbsolutePath().normalize().toString());
			noURLLocation = Paths.get(".").toAbsolutePath().normalize().toString()+"/nourl.html";

		    Logger.getInstance().add(noURLLocation);

		    File nourl = new File(noURLLocation);
		    if(!nourl.exists()){
		       	createNoUrl();
		    }

        
	        int global_offset = 5;
	        int glyph_node_count = 0, base_image_count = 0, link_count = 0, tag_count = 0;

	        // write base images
			boolean world = true;
			int offset = 2;
			File ftest = new File(imageDir);
			if(ftest.exists()){world = false;}	// todo: comment this. what is it for?
			for(int i = 0; i < base_objects.size(); ++i)
			{
				BaseObject bo = base_objects.get(i);
				String[] color = bo.getGridColor();
				int number = i + offset;
				if(i == 0 && (bo.getName().equals("World") || (bo.getType().equals("Downloaded Map") && world))){number = 1;offset = 1;}

				// write texture id
				data.writeInt( number );

				// write transform (todo: why are these stored as strings?! slow af)
				data.writeFloat( Float.parseFloat( bo.getPosition( 0 ) ) );
				data.writeFloat( Float.parseFloat( bo.getPosition( 1 ) ) );
				data.writeFloat( Float.parseFloat( bo.getPosition( 2 ) ) );
				data.writeFloat( Float.parseFloat( bo.getRotation( 0 ) ) );
				data.writeFloat( Float.parseFloat( bo.getRotation( 1 ) ) );
				data.writeFloat( Float.parseFloat( bo.getRotation( 2 ) ) );

				// write color (byte x 3)
				data.write( Integer.parseInt( color[0] ) );
				data.write( Integer.parseInt( color[1] ) );
				data.write( Integer.parseInt( color[2] ) );

				// write grid parameters
				data.writeFloat( (float)bo.getGridLineCount( 0 ) );
				data.writeFloat( (float)bo.getGridLineCount( 1 ) );
				data.writeInt( bo.getGridSegmentsX() );
				data.writeInt( bo.getGridSegmentsY() );

				++base_image_count;
			}

			// write glyph nodes
	        boolean print = true;
	        String lastRootXYZ = "";
	        int firstRoot = 1;
	        excluded = new ArrayList<Integer>();
	        HashMap<Integer,Double> rotation_lookup = new HashMap<Integer,Double>();
	        for(int i = 1; i< nodeCount; i++){
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

	        		data.writeInt( i + global_offset );

			        if(temp.getParent()==0){
			        	data.writeInt( temp.getParent() );
			        }else{
			        	if(rotation_lookup.containsKey(temp.getParent()) && rootCoords.get(firstRoot).toMerge()){
			        		data.writeInt(rootCoords.get(firstRoot).getFirstRootID(lastRootXYZ)+global_offset);
			        	}else{
			        		data.writeInt(temp.getParent()+global_offset);
			        	}
			        }
			        //TRANSLATE
			        if(rootCoords.get(firstRoot).toMerge() && rotation_lookup.containsKey(temp.getParent())){
			        	if(rotation_lookup.get(temp.getParent()) == 0){
				        	data.writeFloat((float)temp.getX());
				        }
				        else
				        {
				        	data.writeFloat((float)rotation_lookup.get(temp.getParent()).doubleValue());
				        }
			        }else{
			        	data.writeFloat((float)temp.getX());
			        }
			        data.writeFloat((float)temp.getY());
			        data.writeFloat((float)temp.getZ());
			        //ROTATE
			        data.writeFloat((float)temp.getRX());
			        data.writeFloat((float)temp.getRY());
			        data.writeFloat((float)temp.getRZ());
			        //SCALE
			        data.writeFloat((float)temp.getSX());
			        data.writeFloat((float)temp.getSY());
			        data.writeFloat((float)temp.getSZ());
			        //COLOR
			        data.write(temp.getCR());
			        data.write(temp.getCG());
			        data.write(temp.getCB());
			        data.write(temp.getAlpha());

			        data.writeInt(temp.getGeo());
			        data.writeInt(temp.getTopo());
			        data.writeFloat((float)temp.getRatio().doubleValue());

			        //ROTATE RATE
			        data.writeFloat((float)temp.getRRX());
			        data.writeFloat((float)temp.getRRY());
			        data.writeFloat((float)temp.getRRZ());

			        data.writeUTF(temp.getTag());
			        data.writeUTF(temp.getURL());
			        data.writeUTF(temp.getDesc());

			        ++glyph_node_count;
			    }else{
			    	excluded.add(i);
			    }
		    	
		    	if(rootCoords.get(firstRoot).getLastID() == i){
		    		firstRoot = rootCoords.get(firstRoot).getLastTemp()+1;
		    	}
		    	print = true;
	        }

			for(Map.Entry<Integer, LinkTemplate> entry : link_temps.entrySet()){
				ArrayList<Integer> parent_ids = entry.getValue().linkParentID();
				ArrayList<Integer> child_ids = entry.getValue().linkChildID();
				int link_index = 0;
				for(int i = 0; i < entry.getValue().linkCount(); i++){
					int p_id = evaluateID(parent_ids.get(link_index));
					int c_id = evaluateID(child_ids.get(link_index));
		
					if(p_id != 0 && c_id != 0)
					{
						data.writeInt(parent_ids.get(link_index)+global_offset);	//id0
						data.writeInt(child_ids.get(link_index)+global_offset);	//id1

						data.writeInt(Integer.parseInt(entry.getValue().getGeo()));

						int cr, cg, cb;
						if(entry.getValue().inheritColor()){ //Capture color from parent
							Node temp = allNodes.get(parent_ids.get(link_index));
							cr = temp.getCR();
							cg = temp.getCG();
							cb = temp.getCB();
						}
						else
						{
							cr = Integer.parseInt(entry.getValue().getCR());
							cg = Integer.parseInt(entry.getValue().getCG());
							cb = Integer.parseInt(entry.getValue().getCB());
						}
						data.write(cr);
						data.write(cg);
						data.write(cb);
						data.write(Integer.parseInt(entry.getValue().getAlpha()));

						++link_count;
					}
					link_index++;
				}
			}

			data.close();
			bufout.close();

			// Write out our counts to make pre allocating storage easier on the C++ side.
			FileOutputStream countfile = new FileOutputStream(outDir+"glyphs.sgn", false); 
			BufferedOutputStream countfileout = new BufferedOutputStream(countfile);
			DataOutputStream countdata = new DataOutputStream(countfileout);
			countdata.writeInt(0x294ee1ac);	// magic number
			countdata.writeInt(base_image_count);
			countdata.writeInt(glyph_node_count);
			countdata.writeInt(link_count);
			countdata.writeInt(tag_count);
			countdata.close();

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
}
