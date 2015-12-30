package synglyphx.util;

public class BoundingBox{
	
	private double[] top_left;
	private double[] bottom_right;
	private double[] top_right;
	private double[] bottom_left;

	public BoundingBox(double[] topleft, double[] bottomright){
		this.top_left = topleft;
		this.bottom_right = bottomright;
		setOpposingCorners();
	}

	public void setOpposingCorners(){
		top_right = new double[2];
		bottom_left = new double[2];

		top_right[0] = bottom_right[0];
		top_right[1] = top_left[1];
		bottom_left[0] = top_left[0];
		bottom_left[1] = bottom_right[1];
	}

	public double[][] getBoundingBox(){
		double[][] boundingBox = new double[2][];
		boundingBox[0] = top_left;
		boundingBox[1] = bottom_right;

		return boundingBox;
	}

/*
	public static void main(String[] args){
		double[] tl = new double[2];
		double[] br = new double[2];
		tl[0] = -180.00;
		tl[1] = 90.00;
		br[0] = 180.00;
		br[1] = -90.00;
		BoundingBox bb = new BoundingBox(tl,br);

		double[][] box = bb.getBoundingBox();
		System.out.println(box[0][0]);
		System.out.println(box[0][1]);
		System.out.println(box[1][0]);
		System.out.println(box[1][1]);
	}
*/
}