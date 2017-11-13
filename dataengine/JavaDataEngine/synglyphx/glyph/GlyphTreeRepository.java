package synglyphx.glyph;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.*;
import synglyphx.io.CSVWriter;
import synglyphx.io.JsonWriter;
import synglyphx.util.BaseObject;
import synglyphx.util.BaseImageSpace;
import synglyphx.util.BoundingBox;
import synglyphx.link.LinkTemplate;

public class GlyphTreeRepository {

	//private static GlyphTreeRepository instance = null;
	private Map<Integer,Node> allNodes = null;
	private ArrayList<Edge> edges = null;
	private int nodeCount;
	private String outDir;
	private String app;
	
	public GlyphTreeRepository(){
		allNodes = new HashMap<Integer,Node>();
		edges = new ArrayList<Edge>();
		nodeCount = 1;
	}
/*
	public static GlyphTreeRepository getInstance(){
		if(instance == null){
			instance = new GlyphTreeRepository();
		}

		return instance;
	}
*/
	public void addNode(Node node){
		allNodes.put(nodeCount, node);
		nodeCount++;
	}

	public void addEdge(int p){
		edges.add(new Edge(p,nodeCount,0));
	}

	public int getNodeCount(){
		return nodeCount;
	}

	public void setOutDir(String outDir, String app){
		this.outDir = outDir;
		this.app = app;
	}

	public void writeAll(String[] colorStr, ArrayList<BaseObject> base_objects, 
		HashMap<Integer, CoordinateMap> rootCoords, boolean scale_zero, 
		Map<Integer, LinkTemplate> link_temps,
		Map<Integer, XMLGlyphTemplate> rootTemplates){

		BaseImageSpace bis = new BaseImageSpace(this.allNodes, this.nodeCount, base_objects, rootCoords);
		if(this.app.equals("Web")){
			JsonWriter csvw = new JsonWriter(this.nodeCount, this.allNodes, outDir, colorStr, app, base_objects, bis.getRootCoords(), scale_zero, link_temps, rootTemplates);
		}else{
			CSVWriter csvw = new CSVWriter(this.nodeCount,this.allNodes, outDir, colorStr, app, base_objects, bis.getRootCoords(), scale_zero, link_temps);
		}
	}

}