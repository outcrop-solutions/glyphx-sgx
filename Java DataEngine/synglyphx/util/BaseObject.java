package synglyphx.util;

public class BaseObject {
	private String type;
	private String name;
	private String mapsource;
	private String maptype;
	private String invert;
	private String grayscale;
	private String bestfit;
	private String margin;
	private String corners;
	private String[] position;
	private String[] rotation;
	private String[] worldsize;
	private String gridLines;
	private double[] gridLineCount;
	private double[] imageSize;
	private int[] gridSegments;
	private String gridColor;
	private double[] logo_pos;
	private BoundingBox boundingBox;
	private BoundingBox geoBoundingBox;

	public BaseObject(){}
	
	public BaseObject(String type){
		this.type = type;
		position = new String[3];
		rotation = new String[3];
		worldsize = new String[2];
		gridLines = "0";
		gridColor = "0,0,0";
		logo_pos = new double[2];
		logo_pos[0] = 167.77;
		logo_pos[1] = 87.23;
	}

	public void setName(String name){
		this.name = name;
	}

	public void setPosition(String x, String y, String z){
		position[0] = x;
		position[1] = y;
		position[2] = z;
	}

	public void setRotation(String x, String y, String z){
		rotation[0] = x;
		rotation[1] = y;
		rotation[2] = z;
	}

	public void setWorldSize(String w, String h){
		worldsize[0] = w;
		worldsize[1] = h;
	}

	public void setGridLines(String gl){
		if(gl.equals("true")){
			this.gridLines = "0";
		}else{
			this.gridLines = "1";
		}
	}

	public void setGridLineCount(String h, String v){
		gridLineCount = new double[2];
		gridLineCount[0] = Double.parseDouble(worldsize[0])/(Integer.parseInt(v)+1);
		gridLineCount[1] = Double.parseDouble(worldsize[1])/(Integer.parseInt(h)+1);
		gridSegments = new int[2];
		gridSegments[0] = Integer.parseInt(h)+1;
		gridSegments[1] = Integer.parseInt(v)+1;
	}

	public void setGridColor(String c){
		this.gridColor = c;
	}

	public void setBoundingBox(BoundingBox geoBB){
		double[] topleft = new double[2];
		double[] bottomright = new double[2];
		topleft[0] = -180.00; topleft[1] = 90.00;
		bottomright[0] = 180.00; bottomright[1] = -90.00;
		boundingBox = new BoundingBox(topleft, bottomright);

		geoBoundingBox = geoBB;
		fitImageToSpace();
	}

	public void setImageSize(double[] imageSize){
		this.imageSize = imageSize;
	}

	public void setMapInfo(String ms, String mt){
		mapsource = ms;
		maptype = mt;
	}

	public void setImageInfo(String inv, String gs, String bf, String mar){
		invert = inv;
		grayscale = gs;
		bestfit = bf;
		margin = mar;
	}

	public void setCornerString(double[] temp){
		corners = String.valueOf(temp[2])+","+String.valueOf(temp[1])+","+String.valueOf(temp[0])+","+String.valueOf(temp[3]);
	}

	public String getPosition(int i){
		return position[i];
	}

	public String getRotation(int i){
		return rotation[i];
	}

	public String getWorldSize(int i){
		return worldsize[i];
	}

	public String getName(){
		return name;
	}

	public String getType(){
		return type;
	}

	public String showGridLines(){
		return gridLines;
	}

	public String[] getGridColor(){
		return gridColor.split(",");
	}

	public String getGLCount(int i){
		if(i == 0 || i == 1){
			return String.valueOf(gridLineCount[i]);
		}else{
			return "0";
		}
	}

	public String getGLSegX(){
		return String.valueOf(gridSegments[1]);
	}

	public String getGLSegY(){
		return String.valueOf(gridSegments[0]);
	}

	public BoundingBox getBoundingBox(){
		return boundingBox;
	}

	public BoundingBox getGeoBoundingBox(){
		return geoBoundingBox;
	}

	public void fitImageToSpace(){

		double image_ratio_w = 2048/imageSize[0];
		double image_ratio_h = 1024/imageSize[1];
		double adjusted = 1.0;

		if(image_ratio_w < image_ratio_h){
			adjusted = imageSize[1]*image_ratio_w;
			gridLineCount[0] = 360.0/gridSegments[1];
			gridLineCount[1] = ((adjusted/gridSegments[0])/1024)*180;
		}
		else if(image_ratio_w > image_ratio_h){
			adjusted = imageSize[0]*image_ratio_h;
			gridLineCount[0] = ((adjusted/gridSegments[1])/2048)*360;
			gridLineCount[1] = 180.0/gridSegments[0];
		}
		else{
			gridLineCount[0] = 180.0/(gridSegments[0]);
			gridLineCount[1] = 360.0/(gridSegments[1]);
		}

		double logo_pos_lon = ((gridLineCount[0]*gridSegments[1])/2)-12.23; 
		double logo_pos_lat = ((gridLineCount[1]*gridSegments[0])/2)-2.77; 

		logo_pos[0] = logo_pos_lon;
		logo_pos[1] = logo_pos_lat;

	}

	public double[] getLogoPos(){
		return logo_pos;
	}

	public String getUpdateCheckString(){
		return corners+" | "+mapsource+","+maptype+" | "+worldsize[0]+","+worldsize[1]+" | "+invert+","+grayscale+" | "+bestfit+","+margin; 
	}

}