package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class ReviewCommitteeOutput implements WebOutput {

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

	public ReviewCommitteeOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "ReviewCommitteeOutput";
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

		if(((index-1)%30) == 0){//ROOT CUBE 8
			scale = new double[]{1.2,1.2,1.2};
			rotate = new double[]{0.0,0.0,0.0};
			geometry = allNodes.get(index+13).getGeo();
			color = String.format("#%02x%02x%02x", allNodes.get(index+13).getCR(), allNodes.get(index+13).getCG(), allNodes.get(index+13).getCB());
		}
		else if(((index-1)%30) == 1){//CENTRAL CYLINDER TOP 9
			pos = new double[]{80.0, 0.0, 0.0};
			scale = new double[]{1.0,1.0,1.7};
			rotate = new double[]{90.0, 90.0, 0.0};
		}
		else if(((index-1)%30) == 4){//CENTRAL CYLINDER BOTTOM RIGHT 12
			pos = new double[]{-65.0, -47.5, 0.0};
			scale = new double[]{1.0,1.0,1.7};
			rotate = new double[]{90.0, 234.0, 0.0};
		}
		else if(((index-1)%30) == 10){//CENTRAL VERTICAL CYLINDER 18
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{1.0,1.0,2.5};
			rotate = new double[]{180.0, 90.0, 0.0};
		}
		else if(((index-1)%30) == 14){//CENTRAL CYLINDER TOP RIGHT 22
			pos = new double[]{25.0, -75.0, 0.0};
			scale = new double[]{1.0,1.0,1.7};
			rotate = new double[]{90.0, 162.0, 0.0};
		}
		else if(((index-1)%30) == 17){//CENTRAL CYLINDER BOTTOM LEFT 25
			pos = new double[]{-65.0, 47.5, 0.0};
			scale = new double[]{1.0,1.0,1.7};
			rotate = new double[]{90.0, -234.0, 0.0};
		}
		else if(((index-1)%30) == 27){//CENTRAL CYLINDER TOP LEFT 35
			pos = new double[]{25.0, 75.0, 0.0};
			scale = new double[]{1.0,1.0,1.7};
			rotate = new double[]{90.0, -162.0, 0.0};
		}

		//CHILDREN OF CENTRAL CYLINDER TOP 9
		else if(((index-1)%30) == 2){//TOP CYLINDER 9 CUBE 10
			pos = new double[]{0.0, 0.0, 30.0};
			scale = new double[]{2.8, 2.8, 1.6};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 3){//TOP CYLINDER 9 CUBE 11
			pos = new double[]{0.0, 0.0, 125.0};
			scale = new double[]{2.8, 2.8, 1.6};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CENTRAL CYLINDER BOTTOM RIGHT 12
		else if(((index-1)%30) == 5){//CYLINDER BOTTOM RIGHT 12 TORUS 13
			pos = new double[]{0.0, 0.0, 55.0};
			scale = new double[]{2.5, 2.5, 1.75};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 6){//CYLINDER BOTTOM RIGHT 12 TORUS 14
			pos = new double[]{0.0, 0.0, 22.5};
			scale = new double[]{2.5, 2.5, 1.75};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 7){//CYLINDER BOTTOM RIGHT 12 TORUS 15
			pos = new double[]{0.0, 0.0, -10.0};
			scale = new double[]{2.5, 2.5, 1.75};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 8){//CYLINDER BOTTOM RIGHT 12 TORUS 16
			pos = new double[]{0.0, 0.0, 87.5};
			scale = new double[]{2.5, 2.5, 1.75};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 9){//CYLINDER BOTTOM RIGHT 12 TORUS 17
			pos = new double[]{0.0, 0.0, 120.0};
			scale = new double[]{2.5, 2.5, 1.75};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CENTRAL VERTICAL CYLINDER 18
		else if(((index-1)%30) == 11){//TOP VERTICAL CYLINDER 18 CYLINDER 19
			pos = new double[]{0.0, 0.0, 90.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 4.0, 0.0, 45.0, scale[0]), Functions.linearInterpolation(0.0, 4.0, 0.0, 45.0, scale[1]), 0.075};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 12){//TOP VERTICAL CYLINDER 18 SPHERE 20
			pos = new double[]{0.0, 0.0, 135.0};
			scale = new double[]{3.0, 3.0, 1.2};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 25){//TOP VERTICAL CYLINDER 18 SPHERE 33
			pos = new double[]{0.0, 0.0, -135.0};
			scale = new double[]{3.0, 3.0, 1.2};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 26){//TOP VERTICAL CYLINDER 18 TORUE 34
			pos = new double[]{0.0, 0.0, -90.0};
			scale = new double[]{3.0, 3.0, 1.25};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CENTRAL CYLINDER TOP RIGHT 22
		else if(((index-1)%30) == 15){//CYLINDER TOP RIGHT 22 OCTA 23
			pos = new double[]{0.0, 0.0, 20.0};
			scale = new double[]{2.8, 2.8, 1.6};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 16){//CYLINDER TOP RIGHT 22 OCTA 24
			pos = new double[]{0.0, 0.0, 120.0};
			scale = new double[]{2.8, 2.8, 1.6};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CENTRAL CYLINDER BOTTOM LEFT 25
		else if(((index-1)%30) == 18){//CYLINDER BOTTOM LEFT 25 ISOS 26
			pos = new double[]{0.0, 0.0, -126.0};
			scale = new double[]{3.0, 3.0, 1.7};
			rotate = new double[]{0.0, 0.0, 0.0};
			topo = 2;
		}
		else if(((index-1)%30) == 23){//CYLINDER BOTTOM LEFT 25 ISOS 31
			pos = new double[]{0.0, 0.0, -20.0};
			scale = new double[]{3.0, 3.0, 1.7};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CENTRAL CYLINDER TOP LEFT 35
		else if(((index-1)%30) == 28){//CYLINDER TOP LEFT 35 CYLINDER 36
			pos = new double[]{0.0, 0.0, -120.0};
			scale = new double[]{12.0, 12.0, 0.5};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%30) == 29){//CYLINDER TOP LEFT 35 CYLINDER 37
			pos = new double[]{0.0, 0.0, -20.0};
			scale = new double[]{12.0, 12.0, 0.5};
			rotate = new double[]{0.0, 0.0, 0.0};
		}

		//CHILDREN OF CYLINDER BOTTOM LEFT 25 ISOS 26
		else if(((index-1)%30) == 19){//CYLINDER BOTTOM LEFT 25 ISOS 26 CUBE 27
			pos = new double[]{-315.0,0.0,90.0};
			scale = new double[]{0.75, 0.75, 0.75};
			rotate = new double[]{0.0,-45.0,0.0};
		}
		else if(((index-1)%30) == 20){//CYLINDER BOTTOM LEFT 25 ISOS 26 CUBE 28
			pos = new double[]{-285.0,0.0,90.0};
			scale = new double[]{0.75, 0.75, 0.75};
			rotate = new double[]{0.0,-67.5,0.0};
		}
		else if(((index-1)%30) == 21){//CYLINDER BOTTOM LEFT 25 ISOS 26 CUBE 29
			pos = new double[]{-255.0,0.0,90.0};
			scale = new double[]{0.75, 0.75, 0.75};
			rotate = new double[]{0.0,67.5,0.0};
		}
		else if(((index-1)%30) == 22){//CYLINDER BOTTOM LEFT 25 ISOS 26 CUBE 30
			pos = new double[]{-225.0,0.0,90.0};
			scale = new double[]{0.75, 0.75, 0.75};
			rotate = new double[]{0.0,45.0,0.0};
		}


		if(parent_id%30 == 25){
			parent_id -= 14;
		}

		if(index%30 == 14 || index%30 == 25){
			alpha = 0;
		}

	}

}