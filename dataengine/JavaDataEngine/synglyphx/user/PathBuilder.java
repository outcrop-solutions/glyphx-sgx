package synglyphx.user;

import java.io.File;
import java.io.FilenameFilter;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.Map;
import java.util.HashMap;
import java.util.Arrays;
import java.util.ArrayList;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class PathBuilder {

	private static String DBNAME = "glyphed.db";
	private HashMap<String, Node> sdtNames;
	private ArrayList<File> sdtFiles;
	private ArrayList<UserFile> userFiles;
	private String localPath;
	
	public PathBuilder(String path){

		sdtNames = new HashMap<String, Node>(); //Map SDT Filename to Viz name in sharedvisualization.xml
		this.localPath = path;
	}

	public void resetSharedVisualizationPaths(ArrayList<UserFile> userFiles){ 

		this.userFiles = userFiles;
		try{
			Files.copy(FileSystems.getDefault().getPath(localPath+"/syncedvisualizations.xml"), FileSystems.getDefault().getPath(localPath+"/sharedvisualizations.xml"), StandardCopyOption.REPLACE_EXISTING);
			File file = new File(localPath+"/sharedvisualizations.xml");
			long lastModified = file.lastModified();

			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			ArrayList<Node> toRemove = new ArrayList<Node>();
			NodeList vizs = doc.getElementsByTagName("Visualization");

			for (int i = 0; i < vizs.getLength(); i++) {
				Node viz = vizs.item(i);
				if(hasPermission(viz.getChildNodes().item(0).getNodeValue())){
					sdtNames.put(((Element)viz).getAttribute("name"), viz.getChildNodes().item(0));
				}else{
					toRemove.add(viz);
				}
			}

			setNewVisualizationPaths();

			for(Node node : toRemove){
				node.getParentNode().removeChild(node);
			}

			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			Transformer transformer = transformerFactory.newTransformer();
			DOMSource source = new DOMSource(doc);
			StreamResult result = new StreamResult(file);
			transformer.transform(source, result);

			file.setLastModified(lastModified);

		}catch(Exception e){
	        e.printStackTrace();
        }
	}

	private boolean hasPermission(String path){
		for(UserFile file : userFiles){

			String file_fp = file.getFormattedPath().replaceAll("\\\\", "/");
			String path_fp = path.replaceAll("\\\\", "/");
			if(path_fp.contains(new File(file_fp.split(".zip")[0]).getPath())){
				return true;
			}
		}
		return false;
	}

	private void setNewVisualizationPaths(){

		String[] directories = getLocalDirectories(localPath);

		for(Map.Entry<String, Node> entry : sdtNames.entrySet()){
			String old_path = entry.getValue().getNodeValue();
			String new_path = findFilePath(old_path, directories);
			if(!new_path.equals("")){
				entry.getValue().setTextContent((new File(new_path)).getPath());
			}

		}
	}

	private String findFilePath(String old_path, String[] directories){
		String name = (new File(old_path)).getName();
		//System.out.println(name);
		for(int i = 0; i < directories.length; i++){
			if(old_path.contains(directories[i])){
				String[] split = old_path.split(directories[i], 2);
				//System.out.println(localPath+File.separator+directories[i]+split[1]);
				if((new File(localPath+File.separator+directories[i]+split[1])).exists()){
					return (localPath+File.separator+directories[i]+split[1]);
				}
				else{
					return "";
				}
			}
		}
		return "";
	}

	private String[] getLocalDirectories(String path){

		File file = new File(path);
		String[] directories = file.list(new FilenameFilter() {
		  @Override
		  public boolean accept(File current, String name) {
		    return new File(current, name).isDirectory();
		  }
		});
		//System.out.println(Arrays.toString(directories));
		return directories;
	}

	public void updateSDTPathways(){

		sdtFiles = new ArrayList<File>();
		File base_dir = new File(localPath);
		findSDT(base_dir, sdtFiles);
		System.out.println(sdtFiles.size());
		for(File file : sdtFiles){
			System.out.println(file.toString());
			restructureSDTInnerPaths(file);
		}
		
	}

	private void findSDT(File base, ArrayList<File> sdtFiles){

		if(base.isDirectory()){
			File[] base_files = base.listFiles();
			for(File file : base_files){
				if(file.isDirectory()){
					findSDT(file, sdtFiles);
				}
				else if(file.getName().contains(".sdt")){
					sdtFiles.add(file);
				}
			}
		}
	}

	private void restructureSDTInnerPaths(File file){

		try{
			long lastModified = file.lastModified();

			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			NodeList baseobjects = doc.getElementsByTagName("BaseObject");
			for (int i = 0; i < baseobjects.getLength(); i++) {
				Node baseobject = baseobjects.item(i);
				if(((Element)baseobject).hasAttribute("filename")){
					((Element)baseobject).setAttribute("filename", restructureFilePath(file, ((Element)baseobject).getAttribute("filename")));
				}
			}

			NodeList datasources = doc.getElementsByTagName("Datasource");
			for (int i = 0; i < datasources.getLength(); i++) {
				Node datasource = datasources.item(i);
				if(((Element)datasource).hasAttribute("type")){
					if(((Element)datasource).getAttribute("type").equals("SQLITE3")){
						NodeList host_nodes = ((Element)datasource).getElementsByTagName("Host").item(0).getChildNodes();
						Node host_node = (Node) host_nodes.item(0);
						host_node.setTextContent((new File(localPath+File.separator+DBNAME)).getPath());

						NodeList name_nodes = ((Element)datasource).getElementsByTagName("Name").item(0).getChildNodes();
						Node name_node = (Node) name_nodes.item(0);
						name_node.setTextContent((new File(localPath+File.separator+DBNAME)).getPath());
					}
				}
			}

			NodeList legends = doc.getElementsByTagName("Legend");
			for (int i = 0; i < legends.getLength(); i++) {
				Node legend = legends.item(i);
				legend.getChildNodes().item(0).setTextContent(restructureFilePath(file, legend.getChildNodes().item(0).getNodeValue()));
			}

			TransformerFactory transformerFactory = TransformerFactory.newInstance();
			Transformer transformer = transformerFactory.newTransformer();
			DOMSource source = new DOMSource(doc);
			StreamResult result = new StreamResult(file);
			transformer.transform(source, result);

			file.setLastModified(lastModified);

		}catch(Exception e){
	        e.printStackTrace();
        }
	}

	private String restructureFilePath(File curr_file, String old_path){

		return findFile(new File(curr_file.getParent()), (new File(old_path)).getName()).toString();
	}

	private File findFile(File base, String toFind){

		if(base.isDirectory()){
			File[] base_files = base.listFiles();
			for(File file : base_files){
				if(file.isDirectory()){
					findSDT(file, sdtFiles);
				}
				else if(file.getName().equals(toFind)){
					return file;
				}
			}
		}
		return base;
	}

}