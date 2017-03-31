package synglyphx.data;

import java.util.HashMap;
import java.util.ArrayList;
import java.util.Date;
import java.text.SimpleDateFormat;
import synglyphx.util.Functions;

public class FieldProperties {

	enum Type {
		DEFAULT,
		NUMBER,
		CURRENCY,
		PERCENTAGE,
		DATETIME
	}

	private HashMap<String, Type> typeMap = null;
	private HashMap<String, String> formatConverter = null;

	private String datasourceID;
	private String tableName;
	private String fieldName;
	private Type dataType;
	private int decimalsToDisplay;
	private String symbol;
	private String datetimeFmt;

	public FieldProperties(String id, String tbl, 
		String fld, String type, int decimals, String sym, String dtFmt)
	{
		typeMap = new HashMap<String, Type>();
		typeMap.put("Default", Type.DEFAULT);
		typeMap.put("Number", Type.NUMBER);
		typeMap.put("Currency", Type.CURRENCY);
		typeMap.put("Percentage", Type.PERCENTAGE);
		typeMap.put("Datetime", Type.DATETIME);

		formatConverter = new HashMap<String, String>();
		formatConverter.put("dddd, MMMM dd, yyyy", "EEEE, MMMM dd, yyyy");
		formatConverter.put("MM/dd", "MM/dd");
		formatConverter.put("dd-MMM", "dd-MMM");
		formatConverter.put("MM/dd/yy", "MM/dd/yy");
		formatConverter.put("dd-MMM-yy", "dd-MMM-yy");
		formatConverter.put("dd-MMM-yyyy", "dd-MMM-yyyy");
		formatConverter.put("MMMM dd, yyyy", "MMMM dd, yyyy");
		formatConverter.put("h:mm AP", "h:mm a");
		formatConverter.put("hh:mm", "HH:mm");
		formatConverter.put("hh:mm:ss", "HH:mm:ss");
		formatConverter.put("hh:mm:ss.zzz", "HH:mm:ss.S");

		datasourceID = id;
		tableName = tbl;
		fieldName = fld;
		dataType = typeMap.get(type);
		decimalsToDisplay = decimals;
		symbol = sym;

		String[] dtSplt = dtFmt.split(" h");
		datetimeFmt = formatConverter.get(dtSplt[0]) + (dtSplt.length == 2 ? (" "+formatConverter.get("h"+dtSplt[1])) : "");
	}

	public boolean matchesField(String id, String table, String field){
		return datasourceID.equals(id) && tableName.equals(table) && fieldName.equals(field);
	}

	public String transformData(String value)
	{
		if(dataType == Type.DEFAULT){
			return value;
		}	
		else if(Functions.isNumeric(value)){

			String first = "";
			String last = "";
			double numericValue = Double.parseDouble(value);

			if(dataType == Type.CURRENCY){
				first = symbol;
			}	
			else if(dataType == Type.PERCENTAGE){
				numericValue *= 100;
				last = "%";
			}	
			else if(dataType == Type.DATETIME){
				if(numericValue < new Date().getTime()/1000.0){
					numericValue *= 1000;
				}
				Date dtVal = new Date((long)numericValue);
				return new SimpleDateFormat(datetimeFmt).format(dtVal);
			}

			return first + Functions.formatNumber(numericValue, decimalsToDisplay) + last;
		}
		return value;
	}
/*
	public static void main(String[] args){

		ArrayList<String> dateFormats = new ArrayList<String>();
		dateFormats.add("dddd, MMMM dd, yyyy");
		dateFormats.add("MM/dd"); 
		dateFormats.add("dd-MMM");
		dateFormats.add("MM/dd/yy"); 
		dateFormats.add("dd-MMM-yy");
		dateFormats.add("dd-MMM-yyyy");
		dateFormats.add("MMMM dd, yyyy");

		ArrayList<String> timeFormats = new ArrayList<String>();
		timeFormats.add("h:mm AP");
		timeFormats.add("hh:mm"); 
		timeFormats.add("hh:mm:ss");
		timeFormats.add("hh:mm:ss.zzz");

		for(String dt : dateFormats){
			FieldProperties fp = new FieldProperties("id", "tbl", "fld", "Datetime", 0, "", dt);
			System.out.println(fp.transformData("1475699533601"));
		}

		for(String dt : timeFormats){
			FieldProperties fp = new FieldProperties("id", "tbl", "fld", "Datetime", 0, "", dt);
			System.out.println(fp.transformData("1475699533601"));
		}
		
	}
*/
}