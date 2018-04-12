package synglyphx.io.web;

import synglyphx.glyph.Node;

public interface WebOutput {

	public String name();
	public void setParameters(Node node, int index, int global_offset);
	public String position();
	public String scale();
	public String rotation();
	public String color();
	public String geometry();
	public String id();
	public String parent();
	public String topology();
	public String alpha();
	public String tag();
	public String url();
	
}