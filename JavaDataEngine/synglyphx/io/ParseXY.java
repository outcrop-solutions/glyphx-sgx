package synglyphx.io;

import java.io.File;
import java.sql.*;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import java.util.Map;
import java.util.HashMap;
import java.util.ArrayList;
import synglyphx.util.ConvertHash;
import synglyphx.data.DataStats;
import synglyphx.data.SourceDataInfo;
import synglyphx.jdbc.Table;
import synglyphx.jdbc.BasicTable;
import synglyphx.jdbc.MergedTable;
import synglyphx.jdbc.driver.Driver;
import synglyphx.jdbc.driver.DriverSelector;
import synglyphx.util.ErrorHandler;

public class ParseXY {

	private Map<String, String> directMap = null;
	private ArrayList<String> x_binds = null;
	private ArrayList<String> y_binds = null;

	public ParseXY(String sdtPath){
		readSDT(sdtPath);
	}

	private void readSDT(String sdtPath){

		x_binds = new ArrayList<String>();
		y_binds = new ArrayList<String>();

		try {

			File file = new File(sdtPath);
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();

			NodeList glyphs = doc.getElementsByTagName("Glyph");

			for(int i = 0; i < glyphs.getLength(); i++){
				Node glyph = glyphs.item(i);
				Element element = (Element) glyph;

				if(element.getAttribute("label").equals("0")){
					Element pos = (Element) element.getElementsByTagName("Position").item(0);
					Element x = (Element) pos.getElementsByTagName("X").item(0);
					Element y = (Element) pos.getElementsByTagName("Y").item(0);
					String x_bind_id = ((Element) x.getElementsByTagName("Binding").item(0)).getAttribute("id");
					String y_bind_id = ((Element) y.getElementsByTagName("Binding").item(0)).getAttribute("id");
					x_binds.add(x_bind_id);
					y_binds.add(y_bind_id);
				}
			}
			setInputMap(doc);
			
		}catch(Exception e){
	        try{
	            e.printStackTrace(ErrorHandler.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
        }
	}

	private void setInputMap(Document doc){

		directMap = new HashMap<String,String>();
		ConvertHash convert = new ConvertHash();

		NodeList inputfields = doc.getElementsByTagName("InputField");

		for(int i = 0; i < inputfields.getLength(); i++){
			Node inputfield = inputfields.item(i);
			Element element = (Element) inputfield;
			String id = element.getAttribute("id");
			String table = element.getAttribute("table");
			String field = element.getAttribute("field");
			String code = convert.getHash(id, table, field);
			if(x_binds.contains(code) || y_binds.contains(code)){
				directMap.put(code, field);
			}
		}
	}

	public double[] getMinMax(ArrayList<SourceDataInfo> sdi){

		double[] x_min_max = new double[2];
		x_min_max[0] = 180.0; x_min_max[1] = -180.0;
		double[] y_min_max = new double[2];
		y_min_max[0] = 90.0; y_min_max[1] = -90.0;
		ConvertHash convert = new ConvertHash();

		double[] min_max = new double[4];

		for(int i = 0; i < sdi.size(); i++){
			SourceDataInfo temp = sdi.get(i);
			//if(temp.getType().equals("csv") || temp.getType().equals("sqlite3")){
				HashMap<String, DataStats> ds = temp.getDataFrame().dataStatsModel();
				min_max = findMinMax(temp, x_min_max, y_min_max, convert, ds);
				x_min_max[0] = min_max[0]; x_min_max[1] = min_max[1];
				y_min_max[0] = min_max[2]; y_min_max[1] = min_max[3];

			/*}else{
				try{
			        Driver driver = DriverSelector.getDriver(temp.getType());
            		Class.forName(driver.packageName());
	        		Logger.getInstance().add("Connecting to Server...");

	        		driver.createConnection(temp.getHost(),temp.getUsername(),temp.getPassword());

	        		System.out.println(temp.getQuery());
	        		Table table;
	        		if(temp.isMerged()){
	        			table = new MergedTable(temp.getQuery(), driver);
	        		}else{
	        			table = new BasicTable(temp.getTable(), driver);
	        		}
			        HashMap<String, DataStats> tds = table.getDataStats();
			        min_max = findMinMax(temp, x_min_max, y_min_max, convert, tds);
			        x_min_max[0] = min_max[0]; x_min_max[1] = min_max[1];
					y_min_max[0] = min_max[2]; y_min_max[1] = min_max[3];

					driver.getConnection().close();
			    }catch(SQLException se){
			        try{
			            se.printStackTrace(Logger.getInstance().addError());
			        }catch(Exception ex){}
			    }catch(Exception e){
			        try{
			            e.printStackTrace(Logger.getInstance().addError());
			        }catch(Exception ex){}
	    		}
			}*/
		}

		double[] to_return = new double[4];
		to_return[0] = x_min_max[0]; to_return[1] = y_min_max[1]; 
		to_return[2] = x_min_max[1]; to_return[3] = y_min_max[0];
		
		return to_return;
	}

	public double[] findMinMax(SourceDataInfo temp, double[] x_min_max, double[] y_min_max, ConvertHash convert, HashMap<String, DataStats> ds){

		String id = temp.getID();
		String table = temp.getTable();

		for(Map.Entry<String,DataStats> entry : ds.entrySet()){
			String code = convert.getHash(id,table,entry.getKey());
			if(x_binds.contains(code)){
				if(Double.parseDouble(entry.getValue().getMin()) < x_min_max[0]){
					x_min_max[0] = Double.parseDouble(entry.getValue().getMin());
				}
				if(Double.parseDouble(entry.getValue().getMax()) > x_min_max[1]){
					x_min_max[1] = Double.parseDouble(entry.getValue().getMax());
				}
			}
			else if(y_binds.contains(code)){
				if(Double.parseDouble(entry.getValue().getMin()) < y_min_max[0]){
					y_min_max[0] = Double.parseDouble(entry.getValue().getMin());
				}
				if(Double.parseDouble(entry.getValue().getMax()) > y_min_max[1]){
					y_min_max[1] = Double.parseDouble(entry.getValue().getMax());
				}
			}
		}

		double[] temp_mm = new double[4];
		temp_mm[0] = x_min_max[0]; temp_mm[1] = x_min_max[1];
		temp_mm[2] = y_min_max[0]; temp_mm[3] = y_min_max[1];
		return temp_mm;
	}

	public static void main(String[] args){
		ParseXY temp = new ParseXY("C:\\Users\\Bryan\\Desktop\\New folder\\Crazy_Complicated_Datamap_V3_Anim.sdt");

	}
}