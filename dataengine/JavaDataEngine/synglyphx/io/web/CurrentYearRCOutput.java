package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class CurrentYearRCOutput implements WebOutput {

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

	public CurrentYearRCOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "CurrentYearRCOutput";
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

		if(((index-1)%30) == 0){//ROOT CUBE
			scale = new double[]{1.2,1.2,1.2};
			rotate = new double[]{0.0,-90.0,0.0};
			geometry = allNodes.get(index+12).getGeo();
			color = String.format("#%02x%02x%02x", allNodes.get(index+12).getCR(), allNodes.get(index+12).getCG(), allNodes.get(index+12).getCB());
		}
		else if(((index-1)%30) == 1){//CENTRAL CYLINDER 1
			pos = new double[]{0.0,0.0,0.0};
			scale = new double[]{0.5,0.5,0.85};
			rotate = new double[]{0.0,0.0,90.0};
		}
		else if(((index-1)%30) == 10){//CENTRAL CYLINDER 2
			pos = new double[]{0.0,0.0,0.0};
			scale = new double[]{0.5,0.5,0.85};
			rotate = new double[]{0.0,0.0,0.0};
		}
		else if(((index-1)%30) == 16){//CENTRAL CYLINDER 3
			pos = new double[]{0.0,0.0,0.0};
			scale = new double[]{0.5,0.5,0.85};
			rotate = new double[]{90.0,0.0,0.0};
		}
		else if(((index-1)%30) == 14){//TOP SPHERE
			pos = new double[]{0.0,0.0,130.0};
			scale = new double[]{1.0,1.0,0.6};
		}
		else if(((index-1)%30) == 15){//TOP TORUS
			pos = new double[]{0.0,0.0,85.0};
			scale = new double[]{0.73,0.73,0.73};
		}
		else if(((index-1)%30) == 5){//SIDE TORUS 1
			pos = new double[]{0.0,0.0,105.0};
			scale = new double[]{0.47,0.47,0.43};
		}
		else if(((index-1)%30) == 6){//SIDE TORUS 2
			pos = new double[]{0.0,0.0,90.0};
			scale = new double[]{0.47,0.47,0.43};
		}
		else if(((index-1)%30) == 7){//SIDE TORUS 3
			pos = new double[]{0.0,0.0,75.0};
			scale = new double[]{0.47,0.47,0.43};
		}
		else if(((index-1)%30) == 8){//SIDE TORUS 4
			pos = new double[]{0.0,0.0,120.0};
			scale = new double[]{0.47,0.47,0.43};
		}
		else if(((index-1)%30) == 9){//SIDE TORUS 5
			pos = new double[]{0.0,0.0,135.0};
			scale = new double[]{0.47,0.47,0.43};
		}
		else if(((index-1)%30) == 2){//SIDE CUBE 1
			pos = new double[]{0.0,0.0,-85.0};
			scale = new double[]{0.6,0.6,0.4};
		}
		else if(((index-1)%30) == 3){//SIDE CUBE 2
			pos = new double[]{0.0,0.0,-130.0};
			scale = new double[]{0.6,0.6,0.4};
		}

		else if(((index-1)%30) == 25){//SIDE CYLINDER 1
			pos = new double[]{0.0,0.0,150.0};
			scale = new double[]{2.75,2.75,0.05};
		}
		else if(((index-1)%30) == 26){//SIDE CYLINDER 2
			pos = new double[]{0.0,0.0,130.0};
			scale = new double[]{2.75,2.75,0.05};
		}
		else if(((index-1)%30) == 27){//SIDE CYLINDER 3
			pos = new double[]{0.0,0.0,110.0};
			scale = new double[]{2.75,2.75,0.05};
		}
		else if(((index-1)%30) == 28){//SIDE CYLINDER 4
			pos = new double[]{0.0,0.0,90.0};
			scale = new double[]{2.75,2.75,0.05};
		}
		else if(((index-1)%30) == 29){//SIDE CYLINDER 5
			pos = new double[]{0.0,0.0,70.0};
			scale = new double[]{2.75,2.75,0.05};
		}

		else if(((index-1)%30) == 17){//ISO 1
			pos = new double[]{0.0,0.0,-82.5};
			scale = new double[]{0.65,0.65,0.4};
		}
		else if(((index-1)%30) == 18){//ISO 2
			pos = new double[]{0.0,0.0,-122.5};
			scale = new double[]{0.65,0.65,0.4};
			topo = 2;
		}
		else if(((index-1)%30) == 23){//OCTA
			pos = new double[]{0.0,0.0,-160.0};
			scale = new double[]{0.55,0.55,0.4};
		}
		else if(((index-1)%30) == 19){//CUBE OVER ISO 1
			pos = new double[]{-315.0,0.0,90.0};
			scale = new double[]{0.225, 0.225, 0.225};
			rotate = new double[]{0.0,-45.0,0.0};
		}
		else if(((index-1)%30) == 20){//CUBE OVER ISO 2
			pos = new double[]{-285.0,0.0,90.0};
			scale = new double[]{0.225, 0.225, 0.225};
			rotate = new double[]{0.0,-67.5,0.0};
		}
		else if(((index-1)%30) == 21){//CUBE OVER ISO 3
			pos = new double[]{-255.0,0.0,90.0};
			scale = new double[]{0.225, 0.225, 0.225};
			rotate = new double[]{0.0,67.5,0.0};
		}
		else if(((index-1)%30) == 22){//CUBE OVER ISO 4
			pos = new double[]{-225.0,0.0,90.0};
			scale = new double[]{0.225, 0.225, 0.225};
			rotate = new double[]{0.0,45.0,0.0};
		}
		else if(((index-1)%30) == 11){//BOTTOM CYLINDER
			pos = new double[]{0.0,0.0,Functions.linearInterpolation(-75.0, 0.0, -80.0, -130.0, pos[0])};
			scale = new double[]{6.0*scale[0],6.0*scale[1],0.025};
		}

		if(((index-1)%30) != 0){
			scale = new double[]{3.0*scale[0],3.0*scale[1],3.0*scale[2]};
		}

		if(parent_id%30 == 5){
			parent_id -= 3;
		}
		else if(parent_id%30 == 14){
			parent_id -= 3;
		}
		else if(parent_id%30 == 25){
			parent_id -= 8;
		}

		if(index%30 == 5 || index%30 == 13 || index%30 == 14 || index%30 == 25){
			alpha = 0;
		}
	}

}