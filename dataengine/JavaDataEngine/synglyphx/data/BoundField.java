package synglyphx.data;

public abstract class BoundField {
	
	protected String id;
	protected String table;
	protected String field;

	public BoundField(String id, String table, String field) {
		this.id = id;
		this.table = table;
		this.field = field;
	}

	public String id(){
		return id;
	}

	public String table(){
		return table;
	}

	public String field(){
		return field;
	}

	@Override
	public boolean equals(Object v){
		boolean retVal = false;

		if(v instanceof BoundField){
			BoundField ptr = (BoundField) v;
			retVal = ptr.id.equals(this.id) && ptr.table.equals(this.table) && ptr.field.equals(this.field);
		}
		return retVal;
	}
}