package synglyphx.user;

import java.sql.Timestamp;

public class UserFile {

	private int fileType;
	private String vizName;
	private String remotePath;
	private String formattedPath;
	private int securityGroup;
	private Timestamp lastModified;

	public UserFile(String name, String rpath, int s_group, int type){
		fileType = type;
		vizName = name;
		remotePath = rpath;
		securityGroup = s_group;
		formattedPath = rpath.replace('_',' ');
		if(type == 4){
			formattedPath = formattedPath.replace("sharedvisualizations.xml","syncedvisualizations.xml");
		}
	}

	public void setLastModified(long dt){
		this.lastModified = new Timestamp(dt);
	}

	public int getFileType(){
		return fileType;
	}

	public String getVizName(){
		return vizName;
	}

	public String getRemotePath(){
		return remotePath;
	}

	public String getFormattedPath(){
		return formattedPath;
	}

	public int getSecurityGroup(){
		return securityGroup;
	}

	public Timestamp getLastModified(){
		return lastModified;
	}

}