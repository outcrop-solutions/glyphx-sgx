package synglyphx.user;

import java.sql.Timestamp;

public class Institution {

	private int id;
	private String name;
	private Timestamp logoModified;
	private Timestamp dataModified;
	private Timestamp sharedModified;

	public Institution(int id,String name){
		this.id = id;
		this.name = name;
	}

	public int getID(){
		return id;
	}

	public String getName(){
		return name;
	}

	public Timestamp getLogoModified(){
		return logoModified;
	}

	public Timestamp getDBModified(){
		return dataModified;
	}

	public Timestamp getSharedModified(){
		return sharedModified;
	}

	public void setLogoModified(long ts){
		logoModified = new Timestamp(ts);
	}

	public void setDataModified(long ts){
		dataModified = new Timestamp(ts);
	}

	public void setSharedModified(long ts){
		sharedModified = new Timestamp(ts);
	}

}