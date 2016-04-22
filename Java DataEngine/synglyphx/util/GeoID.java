package synglyphx.util;

import java.util.Map;
import java.util.HashMap;

public class GeoID {

	public static Map<String, Integer> shapes = null;
	public static Map<String, Integer> wireframe = null;
	public static Map<String, Integer> topos = null;
	
	public GeoID(){
		shapes = new HashMap<String, Integer>();
		shapes.put("Cube", 1);
		shapes.put("Sphere", 3);
		shapes.put("Cone", 5);
		shapes.put("Torus", 7);
		shapes.put("Dodecahedron", 9);
		shapes.put("Octahedron", 11);
		shapes.put("Tetrahedron", 13);
		shapes.put("Icosahedron", 15);
		shapes.put("Pin", 16);
		shapes.put("Cylinder", 19);

		wireframe = new HashMap<String, Integer>();
		wireframe.put("Cube", 0);
		wireframe.put("Sphere", 2);
		wireframe.put("Cone", 4);
		wireframe.put("Torus", 6);
		wireframe.put("Dodecahedron", 8);
		wireframe.put("Octahedron", 10);
		wireframe.put("Tetrahedron", 12);
		wireframe.put("Icosahedron", 14);
		wireframe.put("Pin", 17);
		wireframe.put("Cylinder", 18);

		topos = new HashMap<String, Integer>();
		topos.put("Cube", 1);
		topos.put("Sphere", 2);
		topos.put("Torus", 3);
		topos.put("Cylinder", 4);
		topos.put("Pin", 5);
		topos.put("Rod", 6);
		topos.put("Point", 7);

	}

	public static int getValue(String shape){
		return shapes.get(shape);
	}

	public static int getValue(String shape, String surface){

		if(surface.equals("Solid")){
			return shapes.get(shape);
		}else{
			return wireframe.get(shape);
		}
	}

	public static int getTopoValue(String topo){
		return topos.get(topo);
	}

}