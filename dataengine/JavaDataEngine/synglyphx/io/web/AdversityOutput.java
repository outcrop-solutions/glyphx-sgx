package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class AdversityOutput implements WebOutput {

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

	public AdversityOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "AdversityOutput";
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

		if(((index-1)%21) == 0){//ROOT SPHERE 8
			scale = new double[]{1.2,1.2,1.2};
			rotate = new double[]{0.0,0.0,0.0};
			geometry = allNodes.get(index+20).getGeo();
			alpha = allNodes.get(index+20).getAlpha();
			color = String.format("#%02x%02x%02x", allNodes.get(index+20).getCR(), allNodes.get(index+20).getCG(), allNodes.get(index+20).getCB());
		}
		else if(((index-1)%21) == 1){//CENTRAL CYLINDER TOP/BOTTOM 9
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{1.6, 1.6, 2.8};
			rotate = new double[]{90.0, 180.0, 0.0};
		}
		else if(((index-1)%21) == 4){//CENTRAL CYLINDER LEFT/RIGHT 12
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{1.6, 1.6, 2.8};
			rotate = new double[]{90.0, 90.0, 0.0};
		}

		//FOUR SPHERES 10, 13, 18, 25
		else if(((index-1)%21) == 2){//CYLINDER TOP/BOTTOM 9 SPHERE 10
			pos = new double[]{0.0, 0.0, -150.0};
			scale = new double[]{1.8, 1.8, 1.03};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 5){//CYLINDER LEFT/RIGHT 12 SPHERE 13
			pos = new double[]{0.0, 0.0, 150.0};
			scale = new double[]{Functions.linearInterpolation(0.1, 0.5, 0.45, 2.25, scale[0]), Functions.linearInterpolation(0.1, 0.5, 0.45, 2.25, scale[1]), Functions.linearInterpolation(0.1, 0.5, 0.26, 1.29, scale[2])};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 10){//CYLINDER TOP/BOTTOM 9 SPHERE 18
			pos = new double[]{0.0, 0.0, 150.0};
			scale = new double[]{1.8, 1.8, 1.03};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 17){//CYLINDER LEFT/RIGHT 12 SPHERE 25
			pos = new double[]{0.0, 0.0, -150.0};
			scale = new double[]{Functions.linearInterpolation(0.2, 0.6, 0.9, 2.7, scale[0]), Functions.linearInterpolation(0.2, 0.6, 0.9, 2.7, scale[1]), Functions.linearInterpolation(0.2, 0.6, 0.52, 1.55, scale[2])};
			rotate = new double[]{0.0,0.0,0.0};
		}

		//CHILDREN OF CYLINDER LEFT/RIGHT 12
		else if(((index-1)%21) == 6){//CYLINDER LEFT/RIGHT 12 TORUS 14
			pos = new double[]{0.0, 0.0, 110.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 7){//CYLINDER LEFT/RIGHT 12 TORUS 15
			pos = new double[]{0.0, 0.0, 90.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 8){//CYLINDER LEFT/RIGHT 12 TORUS 16
			pos = new double[]{0.0, 0.0, 70.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 18){//CYLINDER LEFT/RIGHT 12 TORUS 26
			pos = new double[]{0.0, 0.0, -100.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 19){//CYLINDER LEFT/RIGHT 12 TORUS 27
			pos = new double[]{0.0, 0.0, -80.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}

		//CHILDREN OF CYLINDER TOP/BOTTOM 9
		else if(((index-1)%21) == 3){//CYLINDER TOP/BOTTOM 9 TORUS 11
			pos = new double[]{0.0, 0.0, -110.0};
			scale = new double[]{1.0, 1.0, 1.5};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 11){//CYLINDER TOP/BOTTOM 9 TORUS 19
			pos = new double[]{0.0, 0.0, 65.0};
			scale = new double[]{1.0, 1.0, 1.0};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 12){//CYLINDER TOP/BOTTOM 9 TORUS 20
			pos = new double[]{0.0, 0.0, 80.0};
			scale = new double[]{1.0, 1.0, 1.0};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 13){//CYLINDER TOP/BOTTOM 9 TORUS 21
			pos = new double[]{0.0, 0.0, 95.0};
			scale = new double[]{1.0, 1.0, 1.0};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 14){//CYLINDER TOP/BOTTOM 9 TORUS 22
			pos = new double[]{0.0, 0.0, 110.0};
			scale = new double[]{1.0, 1.0, 1.0};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%21) == 15){//CYLINDER TOP/BOTTOM 9 TORUS 23
			pos = new double[]{0.0, 0.0, 125.0};
			scale = new double[]{1.0, 1.0, 1.0};
			rotate = new double[]{0.0,0.0,0.0};
		}

		if(parent_id%21 == 10){
			parent_id -= 8;
		}
		else if(parent_id%21 == 17){
			parent_id -= 12;
		}

		if(index%21 == 10 || index%21 == 17 || index%21 == 21){
			alpha = 0;
		}
		//9, 16, 20
	}

}