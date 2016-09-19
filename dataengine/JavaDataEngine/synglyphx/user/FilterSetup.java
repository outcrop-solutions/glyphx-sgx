package synglyphx.user;

import java.sql.*;
import java.io.File;
import java.io.Reader;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import org.xml.sax.InputSource;
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
import synglyphx.util.Functions;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;

public class FilterSetup {

	private static FilterSetup instance = null;
	private static boolean first;
	private static String dataID;
	private static Driver driver = null;
	
	protected FilterSetup(){
		first = true;
	}

    public static FilterSetup getInstance() {
        if(instance == null){
            instance = new FilterSetup();
        }
        return instance;
    }

    public void addSDTFile(String sdt_path){
    	if(first){
    		extractDataSource(sdt_path);
    		first = false;
    	}
    }

    public String[] getDistinctValuesForField(String id, String table, String field){

    	try{

    		String query = "SELECT DISTINCT(`"+field+"`) FROM `"+table+"`";
			PreparedStatement pstmt = driver.getConnection().prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        ArrayList<String> distinct = new ArrayList<String>();
	        while(rs.next()){
	  			distinct.add(rs.getString(1));
	        }
		    rs.close();
		    
		    return Functions.arrayListToStringList(distinct);

		}catch(Exception e){
	        e.printStackTrace();
	    }
    	return new String[1];
    }

    private void extractDataSource(String sdtPath){

    	try{
			File file = new File(sdtPath);
			InputStream inputStream = new FileInputStream(file);
			Reader reader = new InputStreamReader(inputStream,"UTF-8");
			InputSource is = new InputSource(reader);

			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();

			Document doc = dBuilder.parse(is);
			doc.getDocumentElement().normalize();

			getDataPaths(doc);

		}catch(Exception e){
	        e.printStackTrace();
        }
    }

    private int getDataPaths(Document doc) throws Exception{

		NodeList start = doc.getElementsByTagName("Datasources");
		Node source = start.item(0);

		NodeList sources = source.getChildNodes();

		int holder = 0;
		for(int i=0; i < sources.getLength(); i++){
			Node node = sources.item(i);
			if(node.getNodeName().equals("Datasource")){
				Element e = (Element) node;
				if(e.getAttribute("type").equals("CSV")){
					return 1;
				}else{
					dataID = e.getAttribute("id");
					String host = getValue("Host", e);
					String user = "";
					String pass = "";
					try{
						user = getValue("Username", e);
						pass = getValue("Password", e);
					}catch(Exception ex){}

					driver = DriverSelector.getDriver(e.getAttribute("type").toLowerCase());
         			Class.forName(driver.packageName());
         			driver.createConnection("jdbc:"+host,user,pass);
				}
			}
		}
		return 0;
	}

	private String getValue(String tag, Element element) {

		NodeList nodes = element.getElementsByTagName(tag).item(0).getChildNodes();
		Node node = (Node) nodes.item(0);

		return node.getNodeValue();

	}

	public static void closeDriverIfOpen(){
      
        try{
            if(!driver.getConnection().isClosed()){
                 driver.getConnection().close();
            }
        }catch(Exception e){
            e.printStackTrace();
        }
   }
}