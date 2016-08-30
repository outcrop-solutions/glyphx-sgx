package synglyphx.link;

import java.util.ArrayList;
import synglyphx.util.GeoID;

public class LinkTemplate {

	class Endpoint {
		public int glyph_temp_id;
		public int datasource_id;
		public String field_name;
	};

	private String color_r;
	private String color_g;
	private String color_b;
	private String alpha;
	private String geo;
	private Endpoint[] endpoints;
	private LinkFunction function = null;
	private ArrayList<Integer> key_node_ids = null;
	private ArrayList<Integer> value_node_ids = null;
	private int link_index;
	private int tag_index;
	private int offset;
	private boolean inherit_color;
	private int last_begin;
	private int last_end;

	public LinkTemplate(){
		endpoints = new Endpoint[2];
		key_node_ids = new ArrayList<Integer>();
		value_node_ids = new ArrayList<Integer>();
		link_index = 0;
		tag_index = 0;
		offset = 0;
		inherit_color = false;
	}

	public void setColor(String color, String inherit){
		if(inherit.equals("parent")){inherit_color = true;}
		String[] rgb = color.split(",");
		this.color_r = rgb[0];
		this.color_g = rgb[1];
		this.color_b = rgb[2];
	}

	public void setAlpha(String a){
		this.alpha = a;
	}

	public void setOffset(){
		this.offset = offset;
	}

	public void setBeginPoint(int id, int ds, String name){
		Endpoint ep = new Endpoint();
		ep.glyph_temp_id = id;
		ep.datasource_id = ds;
		ep.field_name = name;
		endpoints[0] = ep;
	}

	public void setEndPoint(int id, int ds, String name){
		Endpoint ep = new Endpoint();
		ep.glyph_temp_id = id;
		ep.datasource_id = ds;
		ep.field_name = name;
		endpoints[1] = ep;
	}

	public void setLinkFunction(String funct){
		this.function = new LinkFunction(funct);
	}

	public void setGeometry(String g){
		GeoID geo_id = new GeoID();
		geo_id.getValue(g);
		this.geo = String.valueOf(geo_id.getValue(g));
	}

	public void addEndpointIDs(int begin, int end){
		if(key_node_ids.size() != 0){
			if(last_begin == end && last_end == begin)
				return;
		}
		key_node_ids.add(begin);
		value_node_ids.add(end);
		last_begin = begin;
		last_end = end;
	}

	public String getCR(){
		return color_r;
	}

	public String getCG(){
		return color_g;
	}

	public String getCB(){
		return color_b;
	}

	public String getAlpha(){
		return alpha;
	}

	public String getGeo(){
		return geo;
	}

	public boolean inheritColor(){
		return inherit_color;
	}

	public LinkFunction getLinkFunction(){
		return function;
	}

	public int getGlyphTempID(int point){
		return endpoints[point].glyph_temp_id;
	}

	public int getDatasourceID(int point){
		return endpoints[point].datasource_id;
	}

	public String getFieldName(int point){
		return endpoints[point].field_name;
	}

	public int linkCount(){
		return key_node_ids.size();
	}

	public ArrayList<Integer> linkParentID(){
		return key_node_ids;
	}

	public ArrayList<Integer> linkChildID(){
		return value_node_ids;
	}

	public String toString(int id, int offset){
		String out = ""; 
		String key_node_id = String.valueOf(key_node_ids.get(link_index)+offset);
		String value_node_id = String.valueOf(value_node_ids.get(link_index)+offset);
		out += id+",7,"+id+",0,"+key_node_id+",1,"+value_node_id+",";
		out += "0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1.5,0,0,0,0,0,0,0,0,0,";
		out += "0,0,0,0,0,0,0,"+geo+",1,0,0.1,0,"+color_r+","+color_g+","+color_b+","+alpha+",";
		out += "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0,"+id+",420\n";
		link_index++;
		return out;
	}

	public String outputTag(int id, int record_id){
		String out = "";
		out += String.valueOf(id)+","+String.valueOf(record_id)+",";
		out += "0,\"Link "+String.valueOf(tag_index)+"\",\"\"\n";
		tag_index++;
		return out;
	}

}