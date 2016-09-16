package synglyphx.user;

import java.sql.Timestamp;
import java.util.ArrayList;

public class User {

	private int id;
	private String fullName;
	private int group;
	private Timestamp lastModified;
	private Institution institution = null;
	private ArrayList<UserFile> userFiles = null;
	private FileSyncer syncer = null;

	public User(int id,String fn,int g,Timestamp lm){
		this.id = id;
		this.fullName = fn;
		this.group = g;
		this.lastModified = lm;
		userFiles = new ArrayList<UserFile>();
		syncer = new FileSyncer();
	}

	public void setInstitution(int institution_id, String institution_name){
		institution = new Institution(institution_id, institution_name);
	}

	public void addUserFile(String file_name, String rpath, int s_group, int type){
		if(s_group == 1 || group == s_group){
			userFiles.add(new UserFile(file_name, rpath, s_group, type));
		}
	}

	public int fileSyncSetup(String sync_dir, String inst_name){
		return syncer.fileSyncSetup(userFiles, sync_dir, institution);
	}

	public int visualizationsToSync(){
		return syncer.visualizationsToSync();
	}

	public void startSyncingFiles(){
		syncer.startSyncingFiles();
	}

	public int filesSynced(){
		return syncer.filesSynced();
	}

	public String getGlyphEdPath(){
		return syncer.getGlyphEdPath();
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
		return group;
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