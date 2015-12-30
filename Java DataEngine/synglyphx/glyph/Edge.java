package synglyphx.glyph;

public class Edge {

	private int parentId;
	private int childId;
	private int type;

	public Edge(int p, int c, int t){
		this.parentId = p;
		this.childId = c;
		this.type = t;
	}

	public int getParentId(){
		return parentId;
	}

	public int getChildId(){
		return childId;
	}

	public int getType(){
		return type;
	}
}