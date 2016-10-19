package synglyphx.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Collections;
import java.util.Comparator;

public class Grid {

	class Point {
		public int index;
		public double value;
		public double x;
		public double y;

		Point(int i, double val){
			this.index = i;
			this.value = val;
		}
	};

	private ArrayList<Point> column = null;
	private HashMap<Integer, Point> pointMap = null;
	private int width;
	private int height;

	public Grid(ArrayList<String> column){
		this.column = generatePoints(column);
		assignGridPositions();
	}

	public void assignGridPositions(){

		Collections.sort(this.column, new Comparator<Point>() {
			public int compare(Point one, Point two){
				return (int)(two.value - one.value);
			}
		});

		int len = this.column.size();
		this.width = 0;
		this.height = 0;
		if(Math.sqrt(len)-(int)Math.sqrt(len) == 0){
			this.width = (int)Math.sqrt(len);
			this.height = (int)Math.sqrt(len);
		}else{
			this.width = (int)Math.sqrt(len)+1;
			this.height = (int)Math.sqrt(len);
			if(this.width*this.height < len){
				this.height += 1;
			}
		}

		int y = this.height-1;
		for(int i = 0; i < len; i++){
			this.column.get(i).x = i%this.width;
			if(i%this.width == 0 && i != 0){
				y--;
			}
			this.column.get(i).y = y;
		}
/*
		for(Point p : this.column){
			System.out.println("("+p.x+", "+p.y+") : "+p.value);
		}*/
	}

	public ArrayList<Point> generatePoints(ArrayList<String> elems){

		ArrayList<Point> temp = new ArrayList<Point>();
		this.pointMap = new HashMap<Integer, Point>();
		try{
			for(int i = 0; i < elems.size(); i++){
				temp.add(new Point(i, Double.parseDouble(elems.get(i))));
				pointMap.put(i, temp.get(i));
			}
		}catch(NumberFormatException nfe){
			nfe.printStackTrace();
		}
		return temp;
	}

	public double getPointValue(String pos, double y1, double y3, int index){

		double x3 = 1.0;
		double x2 = 0.0;
		if(pos.equals("PositionX")){
			x3 = this.width-1;
			x2 = pointMap.get(index).x;
		}
		else if(pos.equals("PositionY")){
			x3 = this.height-1;
			x2 = pointMap.get(index).y;
		}
		return Functions.linearInterpolation(0,x3,y1,y3,x2);
	}
}