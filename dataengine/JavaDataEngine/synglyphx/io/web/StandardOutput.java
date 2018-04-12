package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.glyph.Node;

public class StandardOutput implements WebOutput {

	private double[] pos;
	private double[] scale;
	private double[] rotate;
	private String color;
	private int geometry;
	private int id;
	private int parent_id;
	private int topo;
	private int alpha;
	private String tag;
	private String url;
	private Map<Integer,Node> allNodes;

	public StandardOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "StandardOutput";
	}

	public void setParameters(Node node, int index, int global_offset){

		this.pos = new double[]{node.getX(), node.getY(), node.getZ()};
		this.scale = new double[]{node.getSX(), node.getSY(), node.getSZ()};
		this.rotate = new double[]{node.getRX(), node.getRY(), node.getRZ()};
		this.color = String.format("#%02x%02x%02x", node.getCR(), node.getCG(), node.getCB());
		this.geometry = node.getGeo();
		this.id = index+global_offset;
		this.parent_id = node.getParent()+global_offset;
		this.topo = node.getTopo();
		this.alpha = node.getAlpha();
		this.tag = node.getTag();
		this.url = node.getURL();
	}

	public String position(){
		return Arrays.toString(pos);
	}

	public String scale(){
		return Arrays.toString(scale);
	}

	public String rotation(){
		return Arrays.toString(rotate);
	}

	public String color(){
		return color;
	}

	public String geometry(){
		return String.valueOf(geometry);
	}

	public String id(){
		return String.valueOf(id);
	}

	public String parent(){
		return String.valueOf(parent_id);
	}

	public String topology(){
		return String.valueOf(topo);
	}

	public String alpha(){
		return String.valueOf(alpha);
	}

	public String tag(){
		return tag;
	}

	public String url(){
		return url;
	}

}