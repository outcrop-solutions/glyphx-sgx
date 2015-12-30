package synglyphx.glyph;

import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import java.io.*;
import synglyphx.io.CSVWriter;
import synglyphx.util.BaseObject;
import synglyphx.util.BaseImageSpace;
import synglyphx.util.BoundingBox;

public class GlyphTreeRepository {

	//private static GlyphTreeRepository instance = null;
	private Map<Integer,Node> allNodes = null;
	private ArrayList<Edge> edges = null;
	private int nodeCount = 1;
	private String outDir;
	private String app;
	
	public GlyphTreeRepository(){
		allNodes = new HashMap<Integer,Node>();
		edges = new ArrayList<Edge>();
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

	public void writeAll(String[] colorStr, ArrayList<BaseObject> base_objects, HashMap<Integer, CoordinateMap> rootCoords){
		BaseImageSpace bis = new BaseImageSpace(this.allNodes, this.nodeCount, base_objects);
		CSVWriter csvw = new CSVWriter(this.nodeCount,this.allNodes, outDir, colorStr, app, base_objects, rootCoords);
	}

}