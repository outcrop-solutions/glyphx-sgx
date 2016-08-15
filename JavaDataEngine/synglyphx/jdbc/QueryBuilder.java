package synglyphx.jdbc;

import java.io.File;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class QueryBuilder {
	
	public QueryBuilder(){

	}

	public static String build(String dbtype, String tblname, Node qe){
		String query = "SELECT * FROM "+tblname;
		if(qe != null){
			try{
				return query + logicalStatement(qe);

			}catch(Exception e){
				e.printStackTrace();
			}
		}
		return query;
	}

	private static String logicalStatement(Node qe) throws Exception{
		String whereStmt = " WHERE";
		NodeList stmts = qe.getChildNodes();
		for(int i=0; i<stmts.getLength(); i++){
			Node node = stmts.item(i);
			whereStmt = createStatement(whereStmt, node);
		}
		return whereStmt;
	}

	private static String createStatement(String whereStmt, Node node) throws Exception{
		if(node.getNodeType() == Node.ELEMENT_NODE) {
			Element e = (Element) node;
			if(node.getNodeName().equals("Statement")){
				whereStmt += (" "+e.getAttribute("columnname")+getOperator(e.getAttribute("operator"))+"'"+e.getAttribute("value")+"'");
			}
			else if(node.getNodeName().equals("Group")){
				whereStmt += " "+e.getAttribute("logic").toUpperCase();
				NodeList stmts = e.getChildNodes();
				boolean op = openParenthesis(stmts);
				if(op){
					whereStmt += " (";
				}
				for(int i=0; i<stmts.getLength(); i++){
					Node n = stmts.item(i);
					whereStmt = createStatement(whereStmt, n);
				}
				if(op){
					whereStmt += " )";
				}
			}
		}
		return whereStmt;
	}

	private static String getOperator(String operator){
		if(operator.equals("eq")){
			return "=";
		}
		return "=";
	}

	private static boolean openParenthesis(NodeList stmts){
		for(int i=0; i<stmts.getLength(); i++){
			Node n = stmts.item(i);
			if(n.getNodeName().equals("Group")){
				return true;
			}
		}
		return false;
	}
	/*
	public static void main(String args[]){
		try{
			File file = new File("qbtest.txt");
			DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
			Document doc = dBuilder.parse(file);
			doc.getDocumentElement().normalize();
			try {
				NodeList start = doc.getElementsByTagName("Query");
				System.out.println(QueryBuilder.build("MYSQL","Table1",start));

			}catch(Exception ee){
		        ee.printStackTrace();
	        }
			
		}catch(Exception e){
	        e.printStackTrace();
        }
	}
	*/
}
