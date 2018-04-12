package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import java.util.List;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class FirstSourceOutput implements WebOutput {

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

	public FirstSourceOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "FirstSourceOutput";
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

		if(((index-1)%50) == 0){//ROOT SPHERE 8
			scale = new double[]{1.2,1.2,0.47};
			rotate = new double[]{0.0,90.0,0.0};
		}
		else if(((index-1)%50) == 1){//CENTRAL CYLINDER TOP 9
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 2.4};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%50) == 16){//CENTRAL CYLINDER RIGHT 24
			pos = new double[]{-90.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{0.0, 90.0, 90.0};
		}
		else if(((index-1)%50) == 21){//CENTRAL CYLINDER BOTTOM 29
			pos = new double[]{195.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{0.0, -22.5, 90.0};
		}
		else if(((index-1)%50) == 30){//CENTRAL CYLINDER BOTTOM 38
			pos = new double[]{165.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{0.0, 22.5, 90.0};
		}
		else if(((index-1)%50) == 34){//CENTRAL CYLINDER LEFT 42
			pos = new double[]{105.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{180.0, 67.5, 90.0};
		}
		else if(((index-1)%50) == 38){//CENTRAL CYLINDER LEFT 46
			pos = new double[]{75.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{0.0, 112.5, 90.0};
		}
		else if(((index-1)%50) == 45){//CENTRAL CYLINDER RIGHT 53
			pos = new double[]{-45.0, 0.0, 0.0};
			scale = new double[]{0.4, 0.4, 0.8};
			rotate = new double[]{0.0, 45.0, 90.0};
		}
		else if(((index-1)%50) == 44){//CENTRAL TORUS TOP 52
			pos = new double[]{-90.0, 90.0, 0.0};
			scale = new double[]{0.7, 0.7, 0.7};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		List<Integer> longArm = Arrays.asList(new Integer[]{2,3,4,5,6,7,8,9,10,11,12,13,14,15});
		List<Integer> shortArm = Arrays.asList(new Integer[]{17,18,19,20,22,23,24,25,26,27,28,29,31,32,33,35,36,37,39,40,41,42,43,46,47,48,49});

		if(longArm.contains((index-1)%50)){
			pos = new double[]{0.0, 0.0, (-10+((((index-1)%50)-2)*15))};
			//scale = new double[]{15.68, 7.68, 1.28};
			scale = new double[]{Functions.linearInterpolation(0.0, 2.0, 0.0, 15.68, scale[0]), Functions.linearInterpolation(0.0, 4.5, 0.0, 7.68, scale[1]), Functions.linearInterpolation(0.0, 2.0, 0.0, 1.28, scale[2])};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(shortArm.contains((index-1)%50)){
			//scale = new double[]{15.68, 7.68, 3.84};
			scale = new double[]{Functions.linearInterpolation(0.0, 2.0, 0.0, 15.68, scale[0]), Functions.linearInterpolation(0.0, 4.5, 0.0, 7.68, scale[1]), Functions.linearInterpolation(0.0, 2.0, 0.0, 3.84, scale[2])};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		if((index-1)%50 >= 17 && (index-1)%50 <= 20){
			pos = new double[]{0.0, 0.0, ((((index-1)%50)-17)*60)};
		}
		else if((index-1)%50 >= 22 && (index-1)%50 <= 29){
			pos = new double[]{0.0, 0.0, (10-((((index-1)%50)-22)*40))};
		}
		else if((index-1)%50 >= 31 && (index-1)%50 <= 33){
			pos = new double[]{0.0, 0.0, (10-((((index-1)%50)-31)*60))};
		}
		else if((index-1)%50 >= 35 && (index-1)%50 <= 37){
			pos = new double[]{0.0, 0.0, (10-((((index-1)%50)-35)*60))};
		}
		else if((index-1)%50 >= 39 && (index-1)%50 <= 43){
			pos = new double[]{0.0, 0.0, (10-((((index-1)%50)-39)*60))};
		}
		else if((index-1)%50 >= 46 && (index-1)%50 <= 49){
			pos = new double[]{0.0, 0.0, ((((index-1)%50)-46)*60)};
		}
	}

}