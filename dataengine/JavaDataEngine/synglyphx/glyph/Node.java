package synglyphx.glyph;

import java.util.Map;
import java.util.HashMap;
import synglyphx.util.Functions;

public class Node {
	private double pos_x;
	private double pos_y;
	private double pos_z;
	private double rot_x;
	private double rot_y;
	private double rot_z;
	private double scale_x;
	private double scale_y;
	private double scale_z;
	private int color_r;
	private int color_g;
	private int color_b;
	private int alpha;
	private int geometry;
	private int surface;
	private Double ratio;
	private int topo;
	private String tag;
	private String desc;
	private String url;
	private int tagPos;
	private double rot_rate_x;
	private double rot_rate_y;
	private double rot_rate_z;
	private int numChildren;
	private int childOf;
	private int lineNum;
	private int branchLevel;
	private int template;
	private int label;
	private Map<Integer, Node> children = null; 

	public Node(){
		this.numChildren = 0;
		setChildOf(0);
		setAlpha(255);
		this.url = "nourl.html";
		this.desc = "";
		template = 0;
	}

//SETTERS
	public void setDefaultTagValue(String tag){
		this.tag = tag;
	}

	public void setTemplate(int t){
		template = t;
	}

	public void setTagPos(int pos){
		this.tagPos = pos;
	}

	public void setBranchLevel(int bl){
		this.branchLevel = bl;
	}

	public void setLineNum(int n){
		this.lineNum = n;
	}

	public void setX(double x){
		this.pos_x = x;
	}

	public void setY(double y){
		this.pos_y = y;
	}

	public void setZ(double z){
		this.pos_z = z;
	}

	public void setSX(double x){
		this.scale_x = x;
	}

	public void setSY(double y){
		this.scale_y = y;
	}

	public void setSZ(double z){
		this.scale_z = z;
	}

	public void setRX(double x){
		this.rot_x = x;
	}

	public void setRY(double y){
		this.rot_y = y;
	}

	public void setRZ(double z){
		this.rot_z = z;
	}

	public void setRRX(double x){
		this.rot_rate_x = x;
	}

	public void setRRY(double y){
		this.rot_rate_y = y;
	}

	public void setRRZ(double z){
		this.rot_rate_z = z;
	}

	public void setColor(double h, double s, double v){
		double[] rgb = Functions.convertHSVtoRGB(h,s,v);
		this.color_r = (int)rgb[0];
		this.color_g = (int)rgb[1];
		this.color_b = (int)rgb[2];
	}

	public void setAlpha(int a){
		this.alpha = a;
	}

	public void setGeometry(int g){
		this.geometry = g;
	}

	public void setSurface(String s){
		if(s.equals("Solid")){
			this.surface = 1;
		}else{
			this.surface = 0;
		}
	}

	public void setRatio(Double r){
		this.ratio = r;
	}

	public void setTopo(int t){
		this.topo = t;
	}

	public void setChildOf(int c){
		this.childOf = c;
	}

	public void setTag(String t){
		this.tag = t;
	}

	public void setDesc(String d){
		this.desc = d;
	}

	public void setURL(String u){
		this.url = u;
	}

	public void setNumChildren(int i){
		this.numChildren = i;
	}

	public void setLabel(int l)
	{
		label = l;
	}
//END SETTERS

//GETTERS
	public int getTemplate(){
		return template;
	}

	public int getTagPos(){
		return tagPos;
	}

	public int getBranchLevel(){
		return branchLevel;
	}

	public int getLineNum(){
		return lineNum;
	}

	public int getNumChildren(){
		return this.numChildren;
	}

	public double getX(){
		return pos_x;
	}

	public double getY(){
		return pos_y;
	}

	public int getGeo(){
		return geometry;
	}

	public int getSurface(){
		return surface;
	}

	public Double getRatio(){
		return ratio;
	}

	public int getCR(){
		return color_r;
	}

	public int getCG(){
		return color_g;
	}

	public int getCB(){
		return color_b;
	}

	public int getAlpha(){
		return alpha;
	}

	public int getTopo(){
		return topo;
	}

	public int getParent(){
		return childOf;
	}

	public double getZ(){
		return pos_z;
	}

	public double getSX(){
		return scale_x;
	}

	public double getSY(){
		return scale_y;
	}

	public double getSZ(){
		return scale_z;
	}

	public double getRX(){
		return rot_x;
	}

	public double getRY(){
		return rot_y;
	}

	public double getRZ(){
		return rot_z;
	}

	public double getRRX(){
		return rot_rate_x;
	}

	public double getRRY(){
		return rot_rate_y;
	}

	public double getRRZ(){
		return rot_rate_z;
	}

	public String getTag(){
		return tag;
	}

	public String getDesc(){
		return desc;
	}

	public String getURL(){
		return url;
	}

	public int getLabel(){
		return label;
	}
//END GETTERS

	public void addChild(int index, Node child){
		if(children == null){
			children = new HashMap<Integer,Node>();
		}
		children.put(index,child);
	}
}