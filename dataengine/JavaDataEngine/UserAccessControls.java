
import java.sql.*;
import java.util.Date;
import java.util.ArrayList;
import java.io.File;
import java.io.PrintWriter;
import java.security.MessageDigest;
import synglyphx.user.User;
import synglyphx.user.Syncer;
import synglyphx.user.UserFile;
import synglyphx.user.SecurityGroup;
import synglyphx.user.PathBuilder;
import synglyphx.user.FilterSetup;
import synglyphx.user.ConnectionSpawner;
import synglyphx.io.Logger;
import java.util.concurrent.TimeUnit;

public class UserAccessControls {

	private static User loggedInUser = null;
	private static Connection conn = null;
	private static Syncer syncer = null;

	public static boolean initConnection(){
		
		conn = ConnectionSpawner.spawnConnection();
		return conn != null;
	}
	
	public static int validateCredentials(String username, String password, String appVersion){

		if(username.contains(" ")){
			return 0;
		}
		
		try{
		    String query = "SELECT * FROM ";
		    query += "(UserAccounts INNER JOIN Institutions ON (UserAccounts.Institution=Institutions.ID)) ";
		    query += "WHERE UserAccounts.Email='"+username+"';";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        if(rs.next()){
	        	String pw = rs.getString("UserAccounts.Password");
	        	if(pw.equals(password) || pw.equals(hashPassword(password))){
	        		loggedInUser = new User(rs.getInt("UserAccounts.ID"),rs.getString("UserAccounts.Name"),rs.getTimestamp("UserAccounts.LastLogin"));
					loggedInUser.setInstitution(rs.getInt("UserAccounts.Institution"),rs.getString("Institutions.Name"));
					String update = "UPDATE UserAccounts SET "+
									"LoginCount = "+String.valueOf(rs.getInt("UserAccounts.LoginCount")+1)+", "+
									"LastLogin = '"+(new Timestamp(System.currentTimeMillis())).toString()+"', "+
									"VersionNum = '"+appVersion+"' "+
									"WHERE ID = "+String.valueOf(loggedInUser.getID());
					conn.prepareStatement(update).executeUpdate();
				}
			}
			rs.close();
			pstmt.close();

			if(loggedInUser == null){
				return 0;
			}

			if(checkAvailableGroups() == 1){
				setChosenGroup(loggedInUser.getGroup());
		    }

		}catch(Exception e){
			try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
			return 2;
		}
		return 1;
	}

	public static boolean generateLicenseKey(String location){
		try{
			ResultSet rs = conn.prepareStatement("SELECT * FROM UsageLicenses WHERE `UserID` = "+String.valueOf(loggedInUser.getID())).executeQuery();
			if(rs.next()){
				loggedInUser.setLicenseType(rs.getInt("Type"));
				PrintWriter writer = new PrintWriter(new File(location,"SGXLicense"), "UTF-8");
    			writer.println(rs.getString("Key"));
    			writer.close();
			}
			if(loggedInUser.securityGroupCount() <= 1){
				conn.close();
			}
			return true;
		}
		catch(Exception e){
			try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
		}
		return false;
	}

	public static int checkAvailableGroups(){

		if(loggedInUser.securityGroupCount() == 0){
			try{
				String query = "SELECT AvailableGroups.Group, SecurityGroups.Name, SecurityGroups.S3Directory, Institutions.Name FROM ";
				query += "(AvailableGroups INNER JOIN SecurityGroups ON (SecurityGroups.ID=AvailableGroups.Group)) ";
				query += "INNER JOIN Institutions ON (Institutions.ID=SecurityGroups.Institution) ";
				query += "WHERE UserID="+loggedInUser.getID();
				PreparedStatement pstmt = conn.prepareStatement(query);
		        ResultSet rs = pstmt.executeQuery();

		        while(rs.next()){
		        	int id = rs.getInt("AvailableGroups.Group");
		        	String sgn = rs.getString("SecurityGroups.Name");
		        	String in = rs.getString("Institutions.Name");
		        	String sd = rs.getString("SecurityGroups.S3Directory");
		        	loggedInUser.addSecurityGroup(new SecurityGroup(id, sgn, in, sd));
				}
				rs.close();
				pstmt.close();
			}catch(Exception e){
				try{
	            	e.printStackTrace(Logger.getInstance().addError());
		        }catch(Exception ex){}
		        e.printStackTrace();
			}
		}
		return loggedInUser.securityGroupCount();
	}

	public static String[] getListOfFormattedGroupNames(){
		return loggedInUser.getListOfFormattedGroupNames();
	}

	public static void setChosenGroup(int id){

		setChosenGroupById(loggedInUser.setSelectedGroup(id));
	}

	public static void setChosenGroup(String fgroup){

		setChosenGroupById(loggedInUser.setSelectedGroup(fgroup));
	}

	private static void setChosenGroupById(int id){

		try{
			if(conn.isClosed()){
				conn = ConnectionSpawner.spawnConnection();
			}
			String query = "SELECT VisualizationGroups.Group, Visualizations.Name, Visualizations.Path FROM ";
			query += "(VisualizationGroups INNER JOIN Visualizations ON (VisualizationGroups.VizID=Visualizations.ID)) ";
			query += "WHERE VisualizationGroups.Group="+id+";";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        while(rs.next()){
	        	loggedInUser.addUserFile(rs.getString("Visualizations.Name"),rs.getString("Visualizations.Path"),rs.getInt("VisualizationGroups.Group"),1);
	        }

	        rs.close();
			pstmt.close();

			if(loggedInUser.securityGroupCount() > 1){
				conn.close();
			}
		}catch(Exception e){
			try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
		}
	}

	public static void logOutCurrentUser(){
		loggedInUser = null;
		FilterSetup.getInstance().closeDriverIfOpen();
	}

	public static void flushOutFilterSetup(){
		FilterSetup.getInstance().closeDriverIfOpen();
	}

	public static String getGlyphEdPath(){
		return syncer.getFinalDirectory();
	}

	public static boolean fileSyncSetup(final String sync_dir){
		syncer = new Syncer(loggedInUser.getGroupS3Directory(), sync_dir, loggedInUser.getUserFiles());
		return syncer.isSyncingNeeded(loggedInUser.getRemotePaths());
	}

	public static int visualizationsToSync(){
		return syncer.visualizationsToSync();
	}

	public static void startSyncingFiles(){
		syncer.downloadFiles();
	}

	public static long getSyncProgress(){
		Logger.getInstance().add(String.valueOf(syncer.getProgress()));
		return syncer.getProgress();
	}

	public static boolean isDoneSyncing(){
		return syncer.isDone();
	}

	public static int getUserID(){
		return loggedInUser.getID();
	}

	public static int getLicenseType(){
		return loggedInUser.getLicenseType();
	}

	public static String nameOfUser(){
		return loggedInUser.getFullName();
	}

	public static int institutionID(){
		return loggedInUser.getInstitutionID();
	}

	public static String nameOfInstitution(){
		return loggedInUser.getInstitutionName();
	}

	public static String getS3Directory(){
		return loggedInUser.getGroupS3Directory();
	}

	public static String lastModified(){
		return loggedInUser.getLastModified().toString();
	}

	public static String[] visualizationNames(){
		return loggedInUser.getVisualizationNames();
	}

	public static void setVisualizationNames(String path, String[] names){
		PathBuilder pb = new PathBuilder(path);
		ArrayList<UserFile> userFiles = new ArrayList<UserFile>();
		for(int i = 0; i < names.length; i++){
			userFiles.add( new UserFile("",names[i],1,1));
		}
		pb.resetSharedVisualizationPaths(userFiles);
	}

	private static String hashPassword(String new_pw) throws Exception {

  		MessageDigest md = MessageDigest.getInstance("SHA-256");

  		md.update(new_pw.getBytes());
        byte[] byteData = md.digest();

        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < byteData.length; i++) {
         	sb.append(Integer.toString((byteData[i] & 0xff) + 0x100, 16).substring(1));
        }

        return sb.toString();
  	}

	public static void main(String [] args){

		System.out.println(UserAccessControls.initConnection());
		System.out.println(UserAccessControls.validateCredentials("bholster","bholster",""));
		System.out.println(UserAccessControls.nameOfUser());
		System.out.println(UserAccessControls.nameOfInstitution());

		String os = System.getProperty("os.name").toLowerCase();
		String default_path = "C:/ProgramData/SynGlyphX/Content";
		if(os.contains("mac")){
			default_path = "/Users/synglyphx/Library/Application Support/SynGlyphX/Content";
		}
		//UserAccessControls.generateLicenseKey("C:/ProgramData/SynGlyphX");
		//System.out.println(getListOfFormattedGroupNames());
		setChosenGroup(getListOfFormattedGroupNames()[0]);
		
		if(UserAccessControls.fileSyncSetup(default_path)){
			UserAccessControls.startSyncingFiles();
			do{
				try {
			        Thread.sleep(500);
			    } catch (InterruptedException e) {}
			    System.out.println(UserAccessControls.getSyncProgress());
			}while(UserAccessControls.isDoneSyncing() == false);
		}

		System.out.println("Done with the first one...");

		setChosenGroup(getListOfFormattedGroupNames()[0]);
		
		if(UserAccessControls.fileSyncSetup(default_path)){
			UserAccessControls.startSyncingFiles();
			do{
				try {
			        Thread.sleep(500);
			    } catch (InterruptedException e) {}
			    System.out.println(UserAccessControls.getSyncProgress());
			}while(UserAccessControls.isDoneSyncing() == false);
		}

		System.out.println("Done with the second one...");
/*
		System.out.println("");
		String[] vizNames = UserAccessControls.visualizationNames();
		for(int i = 0; i < vizNames.length; i++){
			System.out.println(vizNames[i]);
		}

		String synced_dir = "C:/ProgramData/SynGlyphX/GlyphEd";
		int count = UserAccessControls.fileSyncSetup(synced_dir);

		int viz_count = UserAccessControls.visualizationsToSync();
		UserAccessControls.startSyncingFiles();

		System.out.println(count);
		int i = -1;
		try{
			while(i != count){
				if(UserAccessControls.filesSynced() > i){
					i++;
					if(i == viz_count+1){
						System.out.println("Syncing Images...");
					}
					else if(i == viz_count+2){
						System.out.println("Syncing Data...");
					}
					else if(i == viz_count+3){
						System.out.println("Syncing Shared...");
					}
					else{
						System.out.println("Visualizations synced "+i+" out of "+viz_count);
					}
				}
				TimeUnit.SECONDS.sleep(1);
			}
		}catch(Exception e){
			e.printStackTrace();
		}

		System.out.println("Done syncing");
*/
	}
}