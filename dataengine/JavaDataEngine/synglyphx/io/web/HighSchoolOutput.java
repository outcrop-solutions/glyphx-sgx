package synglyphx.io.web;

import java.util.Map;
import java.util.Arrays;
import synglyphx.util.Functions;
import synglyphx.glyph.Node;

public class HighSchoolOutput implements WebOutput {

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

	public HighSchoolOutput(Map<Integer,Node> allNodes){
		this.allNodes = allNodes;
	}

	public String name(){
		return "HighSchoolOutput";
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

		//ALL STATIC ELEMENTS
		if(((index-1)%52) == 0){//ROOT 
			topo = 12;
			scale = new double[]{15, 15, 0.35};
			rotate = new double[]{0.0, -90.0, 0.0};
		}
		else if(((index-1)%52) == 1){//TOP-GROUP BASE CYLINDER 1
			pos = new double[]{-6.5, -3.75, 1.5};
			scale = new double[]{0.1, 0.4, 0.1};
			rotate = new double[]{270.0, 60.0, 0.0};
		}
		else if(((index-1)%52) == 9){//TOP-GROUP BASE CYLINDER 2
			pos = new double[]{0.0, 9.8, 1.5};
			scale = new double[]{0.1, 0.1, 0.054};
			rotate = new double[]{180.0, 90.0, 90.0};
		}
		else if(((index-1)%52) == 13){//TOP-GROUP BASE CYLINDER 3
			pos = new double[]{7.5, 0.0, 1.5};
			scale = new double[]{0.1, 0.4, 0.1};
			rotate = new double[]{90.0, 0.0, 90.0};
		}
		else if(((index-1)%52) == 18){//TOP-GROUP BASE CYLINDER 4
			pos = new double[]{0.0, -9.8, 1.5};
			scale = new double[]{0.1, 0.4, 0.054};
			rotate = new double[]{90.0, 0.0, 180.0};
		}
		else if(((index-1)%52) == 44){//TOP-GROUP BASE CYLINDER 5
			pos = new double[]{-6.5, 3.75, 1.5};
			scale = new double[]{0.1, 0.1, 0.1};
			rotate = new double[]{0.0, 30.0, 90.0};
		}
		else if(((index-1)%52) == 8){//CENTRAL TORUS
			pos = new double[]{0.0, 0.0, -225.0};
			scale = new double[]{2.8, 2.8, 3.84};
			rotate = new double[]{90.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 19){//ELEMENT OF TGBC 4 - Octahedron
			pos = new double[]{70.0, 0.0, 60.0};
			scale = new double[]{1.44, 4.0, 2.475};
		}
		else if(((index-1)%52) == 20){//ELEMENT OF TGBC 4 - Cube
			pos = new double[]{0.0, 0.0, 60.0};
			scale = new double[]{1.44, 4.0, 2.97};
		}
		else if(((index-1)%52) == 21){//ELEMENT OF TGBC 4 - Cube
			pos = new double[]{-70.0, 0.0, 60.0};
			scale = new double[]{1.44, 4.0, 2.97};
		}
		//END ALL STATIC ELEMENTS

		//SIDE PANEL 1
		else if(((index-1)%52) == 25){
			pos = new double[]{-26.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 106.0, 0.0};
		}
		else if(((index-1)%52) == 26){
			pos = new double[]{-19.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 102.0, 0.0};
		}
		else if(((index-1)%52) == 27){
			pos = new double[]{-11.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 98.0, 0.0};
		}
		else if(((index-1)%52) == 28){
			pos = new double[]{-4.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 94.0, 0.0};
		}
		else if(((index-1)%52) == 29){
			pos = new double[]{4.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 86.0, 0.0};
		}
		else if(((index-1)%52) == 30){
			pos = new double[]{11.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 82.0, 0.0};
		}
		else if(((index-1)%52) == 31){
			pos = new double[]{19.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 78.0, 0.0};
		}
		else if(((index-1)%52) == 51){
			pos = new double[]{26.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 74.0, 0.0};
		}
		//END SIDE PANEL 1

		//SIDE PANEL 2
		else if(((index-1)%52) == 32){
			pos = new double[]{-108.75, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 101.0, 90.0};
		}
		else if(((index-1)%52) == 33){
			pos = new double[]{-101.25, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 97.0, 90.0};
		}
		else if(((index-1)%52) == 34){
			pos = new double[]{-93.75, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 93.0, 90.0};
		}
		else if(((index-1)%52) == 35){
			pos = new double[]{-86.25, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 87.0, 90.0};
		}
		else if(((index-1)%52) == 36){
			pos = new double[]{-78.75, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 83.0, 90.0};
		}
		else if(((index-1)%52) == 37){
			pos = new double[]{-71.25, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 79.0, 90.0};
		}
		//END SIDE PANEL 2

		//SIDE PANEL 3
		else if(((index-1)%52) == 38){
			pos = new double[]{75.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 10.0, 0.0};
		}
		else if(((index-1)%52) == 39){
			pos = new double[]{82.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 5.0, 0.0};
		}
		else if(((index-1)%52) == 40){
			pos = new double[]{90.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 41){
			pos = new double[]{97.5, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, -5.0, 0.0};
		}
		else if(((index-1)%52) == 42){
			pos = new double[]{105.0, 79.85, 0.0};
			scale = new double[]{0.045, Functions.linearInterpolation(0.0, 5.5, 0.0, 8.75, scale[1]), 0.02};
			rotate = new double[]{90.0, -10.0, 0.0};
		}
		//END SIDE PANEL 3

		//SIDE PANEL 4
		else if(((index-1)%52) == 43){
			pos = new double[]{180.0, 79.85, 0.0};
			scale = new double[]{0.09, 8.0, 0.02};
			rotate = new double[]{90.0, 90.0, 0.0};
		}
		//END SIDE PANEL 4

		//TOP-GROUP 1
		else if(((index-1)%52) == 10){//ELEMENT OF TGBC 1
			pos = new double[]{0.0, 0.0, -75.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 2.0, 0.0, 18.4, scale[0]), 7.68, 1.92};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 11){//ELEMENT OF TGBC 1
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 2.0, 0.0, 18.4, scale[0]), 7.68, 1.92};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 12){//ELEMENT OF TGBC 1
			pos = new double[]{0.0, 0.0, 75.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 2.0, 0.0, 18.4, scale[0]), 7.68, 1.92};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		//END TOP-GROUP 1

		//TOP-GROUP 2
		else if(((index-1)%52) == 14){//ELEMENT OF TGBC 2
			pos = new double[]{0.0, 0.0, 10.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 10.8, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 15){//ELEMENT OF TGBC 2
			pos = new double[]{0.0, 0.0, -25.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 10.8, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 16){//ELEMENT OF TGBC 2
			pos = new double[]{0.0, 0.0, -60.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 10.8, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 17){//ELEMENT OF TGBC 2
			pos = new double[]{0.0, 0.0, -95.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 10.8, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		//END TOP-GROUP 2

		//TOP-GROUP 3
		else if(((index-1)%52) == 22){//ELEMENT OF TGBC 3
			pos = new double[]{0.0, 0.0, -20.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 40.0, 0.0, 6.4, scale[0]), 4.8, 1.2};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 23){//ELEMENT OF TGBC 3
			pos = new double[]{0.0, 0.0, -60.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 40.0, 0.0, 6.4, scale[0]), 4.8, 1.2};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 24){//ELEMENT OF TGBC 3
			pos = new double[]{0.0, 0.0, -100.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 40.0, 0.0, 6.4, scale[0]), 4.8, 1.2};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		//END TOP-GROUP 3

		//TOP-GROUP 4
		else if(((index-1)%52) == 2){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, -225.0};
			double xz = Functions.linearInterpolation(0.0, 1.15, 0.0, 4.25, scale[0]);
			scale = new double[]{xz, 0.2, xz};
			rotate = new double[]{0.0, 0.0, 30.0};
		}
		else if(((index-1)%52) == 3){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, -40.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 9.6, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 4){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, -5.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 9.6, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 5){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, 30.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 9.6, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 6){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, 65.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 9.6, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 7){//ELEMENT OF TGBC 4
			pos = new double[]{0.0, 0.0, 100.0};
			scale = new double[]{Functions.linearInterpolation(0.0, 3.0, 0.0, 9.6, scale[0]), 4.8, 0.96};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		//END TOP-GROUP 4

		//TOP-GROUP 5
		else if(((index-1)%52) == 45){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, -60.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 46){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, -30.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 47){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, 0.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 48){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, 30.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 49){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, 60.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		else if(((index-1)%52) == 50){//ELEMENT OF TGBC 5
			pos = new double[]{0.0, 0.0, 90.0};
			scale = new double[]{3.2, Functions.linearInterpolation(0.0, 1.5, 0.0, 6.0, scale[0]), 2.9};
			rotate = new double[]{0.0, 0.0, 0.0};
		}
		//END TOP-GROUP 5
	}

}