package synglyphx.user;

import java.sql.Timestamp;
import java.util.ArrayList;

public class User {

	private int id;
	private String fullName;
	private Timestamp lastModified;
	private Institution institution = null;
	private int selectedGroup;
	private ArrayList<SecurityGroup> securityGroups = null;
	private ArrayList<UserFile> userFiles = null; 

	public User(int id,String fn,Timestamp lm){
		this.id = id;
		this.fullName = fn;
		this.lastModified = lm;
		userFiles = new ArrayList<UserFile>();
		securityGroups = new ArrayList<SecurityGroup>();
	}

	public void setInstitution(int institution_id, String institution_name){
		institution = new Institution(institution_id, institution_name);
	}

	public void addUserFile(String file_name, String rpath, int s_group, int type){
		userFiles.add(new UserFile(file_name, rpath, s_group, type));
	}

	public void addSecurityGroup(SecurityGroup sg){
		securityGroups.add(sg);
	}

	public void setSelectedGroup(int g){
		selectedGroup = g;
	}

	public int securityGroupCount(){
		return securityGroups.size();
	}

	public ArrayList<String> getRemotePaths(){
		ArrayList<String> remote_paths = new ArrayList<String>();
		for(UserFile uf : userFiles){
			remote_paths.add(uf.getRemotePath());
		}
		return remote_paths;
	}

	public ArrayList<UserFile> getUserFiles(){
		return userFiles;
	}

	public int getID(){
		return id;
	}

	public String getFullName(){
		return fullName;
	}

	public String getInstitutionName(){
		return institution.getName();
	}

	public int getInstitutionID(){
		return institution.getID();
	}

	public int getGroup(){
		if(securityGroups.size()==1){
			selectedGroup = securityGroups.get(0).getId();
		}
		return selectedGroup;
	}

	public String getGroupS3Directory(){
		for(SecurityGroup group : securityGroups){
			if(group.getId()==selectedGroup){
				return group.getS3Directory();
			}
		}
		return null;
	}

	public Timestamp getLastModified(){
		return lastModified;
	}

	public String[] getVisualizationNames(){

		String[] viz_names = new String[userFiles.size()];
		for(int i = 0; i < userFiles.size(); i++){
			viz_names[i] = userFiles.get(i).getFormattedPath().split(".zip")[0];
		}
		return viz_names;
	}

}