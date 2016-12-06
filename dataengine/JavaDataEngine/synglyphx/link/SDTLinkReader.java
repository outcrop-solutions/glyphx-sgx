package synglyphx.link;

import java.io.File;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import synglyphx.data.SourceDataInfo;
//import synglyphx.util.ConvertHash;
import synglyphx.glyph.XMLGlyphTemplate;

public class SDTLinkReader {

	private HashMap<Integer, LinkTemplate> link_temps = null;
	private ArrayList<SourceDataInfo> dataPaths = null;
	private HashMap<Integer, XMLGlyphTemplate> templates = null;
	private Map<String, String> directMap = null;
	private int rootCount;

	public SDTLinkReader(Document doc,
		HashMap<Integer, XMLGlyphTemplate> templates,
		ArrayList<SourceDataInfo> dataPaths,
		Map<String, String> directMap){

		this.link_temps = new HashMap<Integer, LinkTemplate>();
		this.dataPaths = dataPaths;
		this.templates = templates;
		this.directMap = directMap;
		this.rootCount = 0;
		loadLinks(doc);
	}

	//public SDTLinkReader(){}

	private void loadLinks(Document doc){

		NodeList linksObj = doc.getElementsByTagName("Links");
		
		for (int i = 0; i < linksObj.getLength(); i++) {
			Element linkObject = (Element) linksObj.item(i);
			NodeList links = linkObject.getElementsByTagName("Link");

			for (int j = 0; j < links.getLength(); j++) {
				Node link = links.item(j);
				Element element = (Element) link;

				Node color_node = element.getElementsByTagName("Color").item(0);
				Element color_element = (Element) color_node;
				String inherit = color_element.getAttribute("inherit");
				String color = "";
				color += color_element.getAttribute("R")+",";
				color += color_element.getAttribute("G")+",";
				color += color_element.getAttribute("B");
				String alpha = color_element.getAttribute("A");

				//Node geo_node = element.getElementsByTagName("Geometry").item(0);
				//Element geo_element = (Element) geo_node;
				String shape = getValue("Shape", element);

				//Node points_node = element.getElementsByTagName("Endpoints").item(0);
				//Element points_element = (Element) points_node;

				Node begin_node = element.getElementsByTagName("Begin").item(0);
				Element begin_element = (Element) begin_node;
				String b_label = begin_element.getAttribute("label");
				String b_id = begin_element.getAttribute("id");
				String b_binding = begin_element.getAttribute("binding");	

				Node end_node = element.getElementsByTagName("End").item(0);
				Element end_element = (Element) end_node;
				String e_label = end_element.getAttribute("label");
				String e_id = end_element.getAttribute("id");
				String e_binding = end_element.getAttribute("binding");

				Node funct = element.getElementsByTagName("Function").item(0);
				Element funct_element = (Element) funct;
				String function = funct_element.getAttribute("type");

				LinkTemplate new_link_temp = new LinkTemplate();
				new_link_temp.setColor(color, inherit);
				new_link_temp.setAlpha(alpha);
				new_link_temp.setGeometry(shape);
				new_link_temp.setLinkFunction(function);

				functionHandler(new_link_temp, funct_element);

				int b_glyph_id = findGlyphTemplateId(doc, b_label, b_id);
				int e_glyph_id = findGlyphTemplateId(doc, e_label, e_id);

				//System.out.println(b_glyph_id);
				//System.out.println(e_glyph_id);

				int b_data_id = getDataPathForTemplate(directMap.get(b_binding), b_binding);
				int e_data_id = getDataPathForTemplate(directMap.get(e_binding), e_binding);
				/*System.out.print("b_data_id: ");
				System.out.println(b_data_id);
				System.out.print("e_data_id: ");
				System.out.println(e_data_id);*/

				new_link_temp.setBeginPoint(b_glyph_id, b_data_id, directMap.get(b_binding));
				new_link_temp.setEndPoint(e_glyph_id, e_data_id, directMap.get(e_binding));

				link_temps.put(j+1, new_link_temp);

			}
		}
		//System.out.println("Links have been loaded...");
	}

	private int getDataPathForTemplate(String field, String code){
		
		//ConvertHash convert = new ConvertHash();
		for(int i = 0; i < dataPaths.size(); i++){
			SourceDataInfo tb = dataPaths.get(i);
			//if(convert.getHash(tb.getID(),tb.getTable(),field).equals(code)){
			if(tb.hasBoundField(code)){
				return i;
			}
		}
		return rootCount;
	}

	private int findGlyphTemplateId(Document doc, String label, String id){

		NodeList glyphsObj = doc.getElementsByTagName("Glyphs");
		int tempCount = 0;
		boolean canReturn = false;
		//System.out.println(glyphsObj.getLength());
		for (int i = 0; i < glyphsObj.getLength(); i++) {
			Element glyphObject = (Element) glyphsObj.item(i);
			NodeList glyphs = glyphObject.getElementsByTagName("Glyph");
			//System.out.println(glyphs.getLength());
			for(int j = 0; j < glyphs.getLength(); j++){
				Element glyph = (Element) glyphs.item(j);

				if(glyph.getAttribute("label").equals("0")){
					tempCount++;
					rootCount++;
					//System.out.println(glyph.getAttribute("id"));
					//System.out.println(id);
					if(id.equals(glyph.getAttribute("id"))){
						canReturn = true;
					}

					if(label.equals(glyph.getAttribute("label"))){
						if(canReturn)
							return tempCount;
					}
					NodeList childrenObj = glyph.getElementsByTagName("Children");
					if(childrenObj.getLength() > 0){
						Element children = (Element) childrenObj.item(i);
						//System.out.print("Num children: ");
						//System.out.println(childrenObj.getLength());
						int[] returned = handleChildren(children,tempCount,label);
						if(returned[0] == 1 && canReturn)
							return returned[1];
						else
							tempCount = returned[1];
						
					}
				}
			}
		}
		return tempCount;
	}

	private int[] handleChildren(Element children, int tempCount, String label){
		NodeList childGlyphs = children.getElementsByTagName("Glyph");
		int[] toReturn = new int[2];
		//System.out.println("handleChildren called");
		for(int k = 0; k < childGlyphs.getLength(); k++){
			Element childGlyph = (Element) childGlyphs.item(k);
			tempCount++;
			if(label.equals(childGlyph.getAttribute("label"))){
				toReturn[0] = 1;
				toReturn[1] = tempCount;
				return toReturn;
			}
		}
		toReturn[0] = 0;
		toReturn[1] = tempCount;
		return toReturn;
	}

	private void functionHandler(LinkTemplate link_temp, Element funct_element){

		if(link_temp.getLinkFunction().type().name().equals("MATCHVALUE")){
			return;
		}else{
			//Element pairs_element = (Element) funct_element.getElementsByTagName("Pairs").item(0);
			NodeList pairs = funct_element.getElementsByTagName("Pair");

			if(link_temp.getLinkFunction().type().name().equals("KEYVALUE")){
				keyToValue(link_temp, pairs);
			}
			else if(link_temp.getLinkFunction().type().name().equals("KEYRANGE")){
				rangeToValue(link_temp, pairs);
			}
		}
	}

	private void keyToValue(LinkTemplate link_temp, NodeList pairs){

		for(int i = 0; i < pairs.getLength(); i++){
			Element pair = (Element) pairs.item(i);
			//link_temp.getLinkFunction().addKeyValuePair(getValue("Key", pair), getValue("Value", pair));
			String k = pair.getAttribute("key");
			String v = pair.getAttribute("value");
			//System.out.println(k+", "+v);
			link_temp.getLinkFunction().addKeyValuePair(k, v);
		}
	}

	private void rangeToValue(LinkTemplate link_temp, NodeList pairs){

		for(int i = 0; i < pairs.getLength(); i++){
			Element pair = (Element) pairs.item(i);
			//Element range = (Element) pair.getElementsByTagName("Range").item(0);
			//link_temp.getLinkFunction().addKeyRangePair(getValue("Key", pair), getValue("Min", range), getValue("Max", range));
			String k = pair.getAttribute("key");
			String rn = pair.getAttribute("min");
			String rx = pair.getAttribute("max");
			link_temp.getLinkFunction().addKeyRangePair(k, rn, rx);
		}
	}

	private String getValue(String tag, Element element) {
		NodeList nodes = element.getElementsByTagName(tag).item(0).getChildNodes();
		Node node = (Node) nodes.item(0);
		return node.getNodeValue();
	}

	private String getInput(Element element) {
		Node node = element.getElementsByTagName("Binding").item(0);
		Element ele = (Element) node;
		return ele.getAttribute("fieldId");
	}

	public HashMap<Integer, LinkTemplate> getLinkTemps(){
		return link_temps;
	}

	public static void main(String[] args){

		//String sdtPath = "C:/Users/Bryan/Desktop/url_test.sdt";
		/*String sdtPath = "C:/Users/Bryan/Desktop/ND Bug/View 2 V2 Mockup.sdt";

		try{
			File file = new File(sdtPath);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			HashMap<Integer, XMLGlyphTemplate> templates = new HashMap<Integer, XMLGlyphTemplate>();
			ArrayList<SourceDataInfo> dataPaths = new ArrayList<SourceDataInfo>();
			HashMap<String, String> directMap = new HashMap<String, String>();

			SDTLinkReader link_reader = new SDTLinkReader(doc, templates, dataPaths, directMap);

		}catch(Exception e){
			e.printStackTrace();
		}*/

	}
}