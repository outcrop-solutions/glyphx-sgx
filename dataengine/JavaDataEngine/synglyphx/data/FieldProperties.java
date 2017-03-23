package synglyphx.data;

import java.util.HashMap;
import synglyphx.util.Functions;

public class FieldProperties {

	enum Type {
		DEFAULT,
		NUMBER,
		CURRENCY,
		PERCENTAGE
	}

	private HashMap<String, Type> typeMap = null;

	private String datasourceID;
	private String tableName;
	private String fieldName;
	private Type dataType;
	private int decimalsToDisplay;
	private String symbol;

	public FieldProperties(String id, String tbl, 
		String fld, String type, int decimals, String sym)
	{
		typeMap = new HashMap<String, Type>();
		typeMap.put("Default", Type.DEFAULT);
		typeMap.put("Number", Type.NUMBER);
		typeMap.put("Currency", Type.CURRENCY);
		typeMap.put("Percentage", Type.PERCENTAGE);

		datasourceID = id;
		tableName = tbl;
		fieldName = fld;
		dataType = typeMap.get(type);
		decimalsToDisplay = decimals;
		symbol = sym;
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
			return first + Functions.formatNumber(numericValue, decimalsToDisplay) + last;

		}
		return value;
	}

}