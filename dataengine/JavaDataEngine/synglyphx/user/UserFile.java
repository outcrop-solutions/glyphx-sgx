package synglyphx.user;

import java.sql.Timestamp;

public class UserFile {

	private int fileType;
	private String vizName;
	private String remotePath;
	private String formattedPath;
	private int securityGroup;
	private Timestamp lastModified;

	public UserFile(String name, String rpath, int s_group, Timestamp lastmod, int type){
		fileType = type;
		vizName = name;
		remotePath = rpath;
		securityGroup = s_group;
		lastModified = lastmod;
		formattedPath = rpath.replace('_',' ');
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