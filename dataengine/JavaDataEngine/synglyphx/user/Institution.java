package synglyphx.user;

import java.sql.Timestamp;

public class Institution {

	private int id;
	private String name;
	private Timestamp logoModified;
	private Timestamp dataModified;

	public Institution(int id,String name,Timestamp logo_mod,Timestamp data_mod){
		this.id = id;
		this.name = name;
		logoModified = logo_mod;
		dataModified = data_mod;
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

}