package synglyphx.io;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.*;
import java.util.Arrays;
import java.nio.file.Paths;
import synglyphx.glyph.Node;
import synglyphx.glyph.Edge;
import synglyphx.glyph.XMLGlyphTemplate;
import synglyphx.util.ColorIndex;
import synglyphx.util.BaseObject;
import synglyphx.glyph.CoordinateMap;
import synglyphx.link.LinkTemplate;
import synglyphx.util.ErrorHandler;
import synglyphx.io.web.WebOutput;
import synglyphx.io.web.WebOutputSelector;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class JsonWriter {

	// increment when data format changes. if you change this you have to change
	// the corresponding constant in scenereader.cpp as well (in addition to changing
	// that class to read the new format).
	public static final int FORMAT_VERSION = 2;
	
	private int nodeCount;
	private Map<Integer,Node> allNodes;
	private ArrayList<Edge> edges;
	private HashMap<Integer,String> excelMap;
	private ArrayList<BaseObject> base_objects;
	private ArrayList<String> baseObjectLines;
	private HashMap<Integer, CoordinateMap> rootCoords;
	private Map<Integer, LinkTemplate> link_temps;
	private Map<Integer, XMLGlyphTemplate> rootTemplates;
	private ArrayList<Integer> excluded;
	private String outDir;
	private String imageDir;
	private boolean win;
	private boolean remove_scale_zero;

	public JsonWriter(int nodeCount, Map<Integer,Node> nodes, String outDir,
		String[] colorStr, String app, ArrayList<BaseObject> base_objects,
		HashMap<Integer, CoordinateMap> rootCoords, boolean scale_zero,
		Map<Integer, LinkTemplate> link_temps,
		Map<Integer, XMLGlyphTemplate> rootTemplates){
		this.nodeCount = nodeCount;
		this.allNodes = nodes;
		this.base_objects = base_objects;
		this.rootCoords = rootCoords;
		this.remove_scale_zero = scale_zero;
		this.link_temps = link_temps;
		this.rootTemplates = rootTemplates;

		Logger.getInstance().add(outDir);
		this.outDir = outDir;//+"/scene/";
		this.imageDir = outDir+"/base_image_2.png";

		newBegin(app);

	}

	public void newBegin(String app){

		ColorIndex ci = new ColorIndex();
		
		String final_json = "";

		try{

	        int global_offset = 0;//5+baseObjectLines.size();

	        Logger.getInstance().add(String.valueOf(nodeCount)+" nodes printing...");

	        BaseObject bo = base_objects.get(0);
	        boolean is_geo = bo.getType().equals("Downloaded Map");
	        String bg_imageID = is_geo ? "base_image.png" : "";
	        float bg_width = Float.parseFloat(bo.getGLCount(0))*Integer.parseInt(bo.getGLSegX());
	        float bg_height = Float.parseFloat(bo.getGLCount(1))*Integer.parseInt(bo.getGLSegY());
	        String x_name = rootTemplates.get(1).getInput("PositionX");
			String y_name = rootTemplates.get(1).getInput("PositionY");
			String z_name = rootTemplates.get(1).getInput("PositionZ");

	        String bg = "\"background\":{" +
	        "\"image\": \""+bg_imageID+"\"," + 
	        "\"geo\": "+is_geo+"," + 
	        "\"color\": \"#000000\"," +
	        "\"width\": "+String.valueOf(bg_width)+"," + 
	        "\"height\": "+String.valueOf(bg_height)+"," + 
	        "\"axis\": [\""+x_name+"\",\""+y_name+"\",\""+z_name+"\"]}";
	 		Logger.getInstance().add(bg);

	        WebOutput webOuput = WebOutputSelector.getWebOutput(allNodes);
	        System.out.println(webOuput.name());
	        System.out.println(nodeCount);


	        //ArrayList<Double> xs = new ArrayList<Double>();
	        //ArrayList<Double> ys = new ArrayList<Double>();

	        ArrayList<String> json_nodes = new ArrayList<String>();
	        ArrayList<String> json_urls = new ArrayList<String>();
	        excluded = new ArrayList<Integer>();
	        int i;
	        for(i = 1; i < nodeCount; i++){
	        	Node temp = allNodes.get(i);

	        	//xs.add(temp.getX());
	        	//ys.add(temp.getY());

	        	if(isScaleGTZero(temp)){
	        		
	        		webOuput.setParameters(temp, i, global_offset);
	        		String json_node = "{" +
	        		"\"pos\": "+webOuput.position()+"," +
	        		"\"scale\": "+webOuput.scale()+"," +
	        		"\"rotate\": "+webOuput.rotation()+"," +
	        		"\"color\": \""+webOuput.color()+"\"," +
	        		"\"geometry\": "+webOuput.geometry()+"," +
	        		"\"id\": "+webOuput.id()+"," +
	        		"\"parent_id\": "+webOuput.parent()+"," +
	        		"\"topo\": "+webOuput.topology()+"," +
	        		"\"alpha\": "+webOuput.alpha()+"," +
	        		"\"tag\": \""+webOuput.tag()+"\"}";
	        		json_nodes.add(json_node);

	        		if(!webOuput.url().equals("nourl.html")){
	        			json_urls.add("\""+webOuput.id()+"\": \""+webOuput.url()+"\"");
	        		}

			    }else{
			    	excluded.add(i);
			    }
	        }
	        //System.out.println("Done adding json nodes");
	        //System.out.println(String.valueOf(json_nodes.size()) +" json nodes to add");

	        String url_dict = json_urls.toString().replace("[","{").replace("]","}");
	        final_json += "{"+bg+","+"\"nodes\":"+json_nodes.toString()+","+"\"urls\":"+url_dict+"}";

	        FileWriter file = new FileWriter(outDir+"/data.json");
			BufferedWriter bf = new BufferedWriter(file);
			bf.write(final_json);
			bf.close();
			//System.out.println("Write out json nodes");
	        //printLinks(bf, bfw, i, global_offset);
	        ErrorHandler.getInstance().csvWriterCompleted();

	        //DownloadedMap dlm = new DownloadedMap(base_objects.get(0), xs, ys);

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
}
