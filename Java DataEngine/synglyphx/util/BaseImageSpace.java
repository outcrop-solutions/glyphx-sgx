package synglyphx.util;

import java.util.Map;
import java.util.ArrayList;
import synglyphx.glyph.Node;

public class BaseImageSpace{

	private Map<Integer, Node> nodes;
	private ArrayList<BaseObject> base_objects;
	private BoundingBox original = null;
	private BoundingBox image = null;
	private double width;
	private double height;
/*
	public BaseImageSpace(){
		double[] nw = new double[2];
	 	nw[0] = -139.516; nw[1] = 52.0331;
	 	double[] se = new double[2];
	 	se[0] = -49.5167; se[1] = 14.0432;
		image = new BoundingBox(nw, se);
	}
*/
	public BaseImageSpace(Map<Integer,Node> nodes, int count, ArrayList<BaseObject> base_objects){
		this.nodes = nodes;
		this.base_objects = base_objects;

		setBoundingBoxes();

		if(this.image != null){
			for(int i=1;i<count;i++){
				Node temp = nodes.get(i);
				if(temp.getParent() == 0){
					double[] point = new double[2];
					point[0] = temp.getX();
					point[1] = temp.getY();
					point = restructureNodes(point);
					temp.setX(point[0]);
					temp.setY(point[1]);
				}
			}
		}
	}

	public double[] restructureNodes(double[] point){

		double[][] space = image.getBoundingBox();

		double minLat = space[1][1];
		double minLong = space[0][0];
		double maxLat = space[0][1];
		double maxLong = space[1][0];
		
		double minWidth = (width*-1)/2;
		double maxWidth = (width)/2;
		double minHeight = (height*-1)/2;
		double maxHeight = (height)/2;

		double lon = Functions.linearInterpolation(minLong, maxLong, minWidth, maxWidth, point[0]);
		double lat = Functions.linearInterpolation(minLat, maxLat, minHeight, maxHeight, point[1]);

		double[] output = new double[2];
		output[0] = lon;
		output[1] = lat;

		return output;

	}

	public double[] calculateCenterPoint(double[] topleft, double[] bottomright){
		double[] center = new double[2];
		center[0] = (topleft[0]+bottomright[0])/2;
		center[1] = (topleft[1]+bottomright[1])/2;

		return center;
	}

	private void setBoundingBoxes(){
		for(int i=0;i<base_objects.size();i++){
			BaseObject bObject = base_objects.get(i);
			if(bObject.getType().equals("Downloaded Map") && Double.parseDouble(bObject.getPosition(0)) == 0 && Double.parseDouble(bObject.getPosition(1)) == 0 && Double.parseDouble(bObject.getPosition(1)) == 0){
				this.original = bObject.getBoundingBox();
				this.image = bObject.getGeoBoundingBox();
				this.width = Double.parseDouble(bObject.getGLCount(0))*Double.parseDouble(bObject.getGLSegX());
				this.height = Double.parseDouble(bObject.getGLCount(1))*Double.parseDouble(bObject.getGLSegY());
				break;
			}
		}
	}
/*
	public void testCase(double xx, double yy, double x, double y){
		double[] xy = new double[2];
		xy[0] = xx;
		xy[1] = yy;
		if(restructureNodes(xy)[0] == x && restructureNodes(xy)[1] == y){
			System.out.println("("+String.valueOf(xy[0])+","+String.valueOf(xy[1])+") scales to ("+String.valueOf(x)+","+String.valueOf(y)+") : TRUE");
		}else{
			System.out.println("("+String.valueOf(xy[0])+","+String.valueOf(xy[1])+") scales to ("+String.valueOf(x)+","+String.valueOf(y)+") : FALSE");
		}
	}

	public static void main(String[] args){

		BaseImageSpace temp = new BaseImageSpace();

		temp.testCase(-135,67.5,-90,45);
		temp.testCase(-90,45,0,0);
		temp.testCase(-90,30,0,-30);
		temp.testCase(-125,67.5,-70,45);
		temp.testCase(-145,67.5,-110,45);
		
		double[] p = new double[2];
	 	p[0] = -122.332; p[1] = 39.6062;
		double[] new_point = temp.restructureNodes(p);
		System.out.println(new_point[0]);
		System.out.println(new_point[1]);
	}
*/
}