package synglyphx.glyph;

import java.util.HashMap;
import java.util.ArrayList;

public class CoordinateMap{

	private HashMap<String,ArrayList<Integer>> toMerge;
	private boolean merge;
	private int lastID;
	private int lastTemp;

	public CoordinateMap(boolean m, int lt){
		this.merge = m;
		this.lastTemp = lt;
		toMerge = new HashMap<String,ArrayList<Integer>>();
	}

	public void addCoordinate(Node node, int i){
		String xyz = String.valueOf(node.getX())+String.valueOf(node.getY())+String.valueOf(node.getZ());
		if(!toMerge.containsKey(xyz)){
			toMerge.put(xyz, new ArrayList<Integer>());
		}
		toMerge.get(xyz).add(i);
	}

	public void setLastID(int last){
		lastID = last;
	}

	public boolean toMerge(){
		return merge;
	}

	public int getLastID(){
		return lastID;
	}

	public int getLastTemp(){
		return lastTemp;
	}

	public boolean containsXYZ(String xyz){
		if(toMerge.containsKey(xyz)){
			return true;
		}
		return false;
	}

	public int getFirstRootID(String xyz){
		return toMerge.get(xyz).get(0);
	}

	public int getFirstRootChildCount(String xyz){
		return toMerge.get(xyz).size();
	}

}