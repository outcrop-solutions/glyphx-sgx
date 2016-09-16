
import java.sql.*;
import java.util.Date;
import java.util.ArrayList;
import synglyphx.user.User;
import synglyphx.user.UserFile;
import synglyphx.user.PathBuilder;
import synglyphx.io.Logger;
import java.util.concurrent.TimeUnit;

public class UserAccessControls {

	private static String HOST = "ec2-52-42-56-15.us-west-2.compute.amazonaws.com";
	private static String USER = "sgxuser";
	private static String PASS = "sgxpass";
	private static String DBNAME = "SynGlyphX";
	private static String PORT = "3306";
	private static User loggedInUser = null;
	private static Connection conn;

	public static boolean initConnection(){
		try{
			Class.forName("com.mysql.jdbc.Driver");
			String conn_str = "jdbc:mysql://"+HOST+":"+PORT+"/"+DBNAME;
			conn = DriverManager.getConnection(conn_str, USER, PASS);
			return true;
		}catch(Exception e){
			e.printStackTrace();
			return false;
		}
	}
	
	public static int validateCredentials(String username, String password){

		try{
		    String query = "SELECT * FROM ";
		    query += "(UserAccounts INNER JOIN Institutions ON (UserAccounts.Institution=Institutions.ID)) ";
		    query += "WHERE UserAccounts.Username='"+username+"' && UserAccounts.Password='"+password+"';";
			PreparedStatement pstmt = conn.prepareStatement(query);
	        ResultSet rs = pstmt.executeQuery();

	        if(rs.next()){
	        	loggedInUser = new User(rs.getInt("UserAccounts.ID"),rs.getString("UserAccounts.Name"),rs.getInt("UserAccounts.Group"),rs.getTimestamp("UserAccounts.LastModified"));
				loggedInUser.setInstitution(rs.getInt("UserAccounts.Institution"),rs.getString("Institutions.Name"));
			}
			rs.close();
			pstmt.close();

			if(loggedInUser == null){
				return 0;
			}

			query = "SELECT VisualizationGroups.Group, Visualizations.Name, Visualizations.Path FROM ";
			query += "(VisualizationGroups INNER JOIN Visualizations ON (VisualizationGroups.VizID=Visualizations.ID)) ";
			query += "WHERE VisualizationGroups.Institution="+loggedInUser.getInstitutionID()+" AND VisualizationGroups.Group="+loggedInUser.getGroup()+";";
			pstmt = conn.prepareStatement(query);
	        rs = pstmt.executeQuery();

	        while(rs.next()){
	        	loggedInUser.addUserFile(rs.getString("Visualizations.Name"),rs.getString("Visualizations.Path"),rs.getInt("VisualizationGroups.Group"),1);
	        }
	        rs.close();
			pstmt.close();

			conn.close();

		}catch(Exception e){
			try{
	            e.printStackTrace(Logger.getInstance().addError());
	        }catch(Exception ex){}
	        e.printStackTrace();
			return 2;
		}
		return 1;
	}

	public static void logOutCurrentUser(){
		loggedInUser = null;
	}

	public static String getGlyphEdPath(){
		return loggedInUser.getGlyphEdPath();
	}

	public static int fileSyncSetup(final String sync_dir){
		return loggedInUser.fileSyncSetup(sync_dir, nameOfInstitution());
	}

	public static int visualizationsToSync(){
		return loggedInUser.visualizationsToSync();
	}

	public static void startSyncingFiles(){
		loggedInUser.startSyncingFiles();
	}

	public static int filesSynced(){
		return loggedInUser.filesSynced();
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

	public static void main(String [] args){

		System.out.println(UserAccessControls.initConnection());
		System.out.println(UserAccessControls.validateCredentials("ghbrown","ghbrown"));
		System.out.println(UserAccessControls.nameOfUser());
		System.out.println(UserAccessControls.nameOfInstitution());

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

	}
}