package synglyphx.user;

import java.io.File;
import java.util.HashMap;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class PathBuilder {

	private HashMap<String, String> sdtNames;
	private String localPath;
	
	public PathBuilder(String path){

		sdtNames = new HashMap<String, String>(); //Map SDT Filename to Viz name in sharedvisualization.xml
		this.localPath = path;
	}

	public void resetSharedVisualizationPaths(){

		try{
			File file = new File(localPath+"/sharedvisualizations.xml");
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			NodeList vizs = doc.getElementsByTagName("Visualization");
			for (int i = 0; i < vizs.getLength(); i++) {
				Node viz = vizs.item(i);
				sdtNames.put(viz.getChildNodes().item(0).getNodeValue(), ((Element)viz).getAttribute("name"));
			}

		}catch(Exception e){
	        e.printStackTrace();
        }
	}

	/*
	Next steps:
		Iterate through files in the Institutions pathways and find the sdt specified in the sharedviz file.
		Replace the path in the sharedviz file with the new path to the sdt.
		For each sdt found, replace all paths in sdt to the corresponding local paths. (Images, Data, Legend)
	*/

}