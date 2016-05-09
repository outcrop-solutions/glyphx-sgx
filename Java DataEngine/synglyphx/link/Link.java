package synglyphx.link;

import synglyphx.glyph.Node;

public class Link extends Node{
	
	public Link(){super();}

	public String toString(){
		String out; 
		out += id+",7,"+id+",0,"+to+",1,"+from+",0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,";
		out += "1.5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"+geo+",1,0,0.1,0,"+cr+","+cg+","+cb+","+a+",";
		out += "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,"+id+",420";
		return out;
	}
}