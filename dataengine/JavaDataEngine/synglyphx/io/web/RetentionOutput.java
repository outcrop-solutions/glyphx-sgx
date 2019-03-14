package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class RetentionOutput implements WebOutput {

	private double[] pos;
	private double[] scale;
	private double[] rotate;
	private String color;
	private int geometry;
	private int index;
	private int global_offset;
	private int parent_id;
	private int topo;
	private int alpha;
	private String tag;
	private String url;
	private Map<Integer,Node> allNodes;

	public RetentionOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "RetentionOutput";
	}

	public void setParameters(Node node, int index, int global_offset){

		this.pos = new double[]{node.getX(), node.getY(), node.getZ()};
		this.scale = new double[]{node.getSX(), node.getSY(), node.getSZ()};
		this.rotate = new double[]{node.getRX(), node.getRY(), node.getRZ()};
		this.color = String.format("#%02x%02x%02x", node.getCR(), node.getCG(), node.getCB());
		this.geometry = node.getGeo();
		this.index = index;
		this.global_offset = global_offset;
		this.parent_id = node.getParent()+global_offset;
		this.topo = node.getTopo();
		this.alpha = node.getAlpha();
		this.tag = node.getTag();
		this.url = node.getURL();
		adjustValues();
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
		return String.valueOf(index+global_offset);
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

	private void adjustValues(){

		int ct = 18;

		if(((index-1)%ct) == 0){//ROOT TORUS 1
			scale = new double[]{1.0,1.0,1.0};
			rotate = new double[]{0.0,0.0,0.0};
			geometry = allNodes.get(index+1).getGeo();
			alpha = allNodes.get(index+1).getAlpha();
			color = String.format("#%02x%02x%02x", allNodes.get(index+1).getCR(), allNodes.get(index+1).getCG(), allNodes.get(index+1).getCB());
		}
		else if(((index-1)%ct) == 13){//CENTRAL CUBE 13
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{3.0, 3.0, 0.9};
		}

		//THREE SPHERES 15, 16, 17
		else if(((index-1)%ct) == 15){//SPHERE 15
			pos = new double[]{0.0, 90, 0.0};
			scale = new double[]{2.25, 2.25, 2.25};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%ct) == 16){//SPHERE 16
			pos = new double[]{70, -60, 0.0};
			scale = new double[]{2.25, 2.25, 2.25};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%ct) == 17){//SPHERE 17
			pos = new double[]{-70, -60, 0.0};
			scale = new double[]{2.25, 2.25, 2.25};
			rotate = new double[]{0.0,0.0,0.0};
		}

		//OUTER CUBE AND CONES 10, 11, 12
		else if(((index-1)%ct) == 10){//CONE 10
			pos = new double[]{105, 105, 0.0};
			scale = new double[]{1.5, 1.5, 3.3};
			rotate = new double[]{0.0,-45,-90};
		}
		else if(((index-1)%ct) == 11){//CONE 11
			pos = new double[]{132.5, 70, 0.0};
			scale = new double[]{1.5, 1.5, 3.3};
			rotate = new double[]{0.0,-27.5,-90};
		}
		else if(((index-1)%ct) == 12){//CUBE 12
			pos = new double[]{-105, 105, 0.0};
			scale = new double[]{2.1, 3.0, 0.9};
			rotate = new double[]{0.0,45,0.0};
		}

		//THREE EASY TORUS 2, 8, 9
		else if(((index-1)%ct) == 2){//TORUS 2
			pos = new double[]{-150, 0.0, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,90,90};
		}
		else if(((index-1)%ct) == 8){//TORUS 8
			pos = new double[]{0.0, 150, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,0.0,90};
		}
		else if(((index-1)%ct) == 9){//TORUS 9
			pos = new double[]{150, 0.0, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,90,90};
		}

		//BOTTOM LEFT TORUS 4, 6
		else if(((index-1)%ct) == 4){//TORUS 4
			pos = new double[]{-115, -100, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,48.75,90};
		}
		else if(((index-1)%ct) == 6){//TORUS 6
			pos = new double[]{-50, -145, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,25.75,90};
		}

		//BOTTOM RIGHT TORUS 3, 5, 7
		else if(((index-1)%ct) == 3){//TORUS 3
			pos = new double[]{130, -70, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,-63,90};
		}
		else if(((index-1)%ct) == 5){//TORUS 5
			pos = new double[]{80, -130, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,-34.5,90};
		}
		else if(((index-1)%ct) == 7){//TORUS 7
			pos = new double[]{40, -145, 0.0};
			scale = new double[]{1.0, 2.0, 3.0};
			rotate = new double[]{0.0,-17.25,90};
		}
		

		if(((index-1)%ct) != 0){
			parent_id = index - ((index-1)%ct);
		}

		if(((index-1)%ct) == 1 || ((index-1)%ct) == 14){
			alpha = 0;
		}
	}

}