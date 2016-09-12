package synglyphx.user;

import java.sql.Timestamp;

public class Institution {

	private int id;
	private String name;
	private Timestamp logoModified;
	private Timestamp dataModified;
	private Timestamp sharedModified;

	public Institution(int id,String name){
		this.id = 2;
		//this.name = name;
		this.name = "Notre Dame";
	}

	public void setLogoModified(long dt){
		this.logoModified = new Timestamp(dt);
	}

	public void setSharedModified(long dt){
		this.sharedModified = new Timestamp(dt);
	}

	public void setDataModified(long dt){
		this.dataModified = new Timestamp(dt);
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

}