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

	private static String DBNAME = "data.db";
	private ArrayList<Node> sdtNames;
	private ArrayList<File> sdtFiles;
	private ArrayList<UserFile> userFiles;
	private String localPath;
	
	public PathBuilder(String path){

		sdtNames = new ArrayList<Node>(); //Map SDT Filename to Viz name in sharedvisualization.xml
		this.localPath = path;
	}

	public void resetSharedVisualizationPaths(ArrayList<UserFile> userFiles){ 

		this.userFiles = userFiles;
		try{
			Files.copy(FileSystems.getDefault().getPath(new File(localPath,"syncedvisualizations.xml").getAbsolutePath()), FileSystems.getDefault().getPath(new File(localPath,"sharedvisualizations.xml").getAbsolutePath()), StandardCopyOption.REPLACE_EXISTING);
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
					sdtNames.add(viz.getChildNodes().item(0));
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
			String test_path = new File(file_fp.split(".zip")[0]).getName();//.getPath().replaceAll("\\\\", "/");
			if(path_fp.contains("/"+test_path+"/")){
				return true;
			}
		}
		return false;
	}

	private void setNewVisualizationPaths(){

		String[] directories = getLocalDirectories(localPath);

		for(Node entry : sdtNames){
			String old_path = entry.getNodeValue();
			String new_path = findFilePath(old_path, directories);
			if(!new_path.equals("")){
				entry.setTextContent((new File(new_path)).getPath());
			}

		}
	}

	private String findFilePath(String old_path, String[] directories){

		String[] dpath = old_path.split("/|\\\\");
		String name = dpath[dpath.length-1];

		for(int i = 0; i < directories.length; i++){
			File new_path = new File(localPath+File.separator+directories[i]+File.separator+name);
			if(new_path.exists() && old_path.contains(directories[i])){
				return new_path.getAbsolutePath();
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
		//System.out.println(sdtFiles.size());
		for(File file : sdtFiles){
			//System.out.println(file.toString());
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
				else if(file.getName().endsWith(".sdt")){
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
					String attrfile = ((Element)baseobject).getAttribute("filename");
					String new_path = restructureFilePath(file, attrfile);
					((Element)baseobject).setAttribute("filename", new_path);
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

		if(new File(base.getAbsolutePath(), toFind).exists()){
			return new File(base.getAbsolutePath(), toFind);
		}
		else if(new File(localPath, toFind).exists()){
			return new File(localPath, toFind);
		}
		else{
			File[] base_files = base.listFiles();
			for(File file : base_files){
				if(file.isDirectory()){
					findFile(file, toFind);
				}
				else if(file.getName().equals(toFind)){
					return file;
				}
			}
		}
		return base;
	}

}