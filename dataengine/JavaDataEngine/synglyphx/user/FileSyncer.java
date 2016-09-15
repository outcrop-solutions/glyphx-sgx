package synglyphx.user;

import com.jcraft.jsch.*;
import java.io.*;
import java.util.Map;
import java.util.Hashtable;
import java.util.ArrayList;
import java.util.Vector;
import java.sql.Timestamp;
import synglyphx.io.Logger;

public class FileSyncer {

	//private static String KEY = "C:/Users/Bryan/Desktop/GlyphEd/sgxinstancekey.ppk";
	private static String HOST = "ec2-52-42-56-15.us-west-2.compute.amazonaws.com";
	private static String USER = "ec2-user";
	private static int PORT = 22;
	private String key;
	private ArrayList<UserFile> needToSync = null;
	private Hashtable<File, Long> toUpdateTS = null;
	private String syncedDirPath;
	private String glyphEdPath;
	private JSch jsch = null;
	private Session session = null;
	private int needToSyncImage;
	private int needToSyncData;
	private int needToSyncShared;
	//private byte[] priv_key;
	//private byte[] pub_key;
	private volatile boolean doneSyncing;
	private volatile int files_synced;

	public FileSyncer(){
		needToSyncImage = 0;
		needToSyncShared = 0;
		needToSyncData = 0;
		files_synced = 1;
		doneSyncing = false;
		//setupPrivateKey();
	}
/*
	private void setupPrivateKey(){
		String pub = "AAAAB3NzaC1yc2EAAAADAQABAAABAQDFasQ7Wttz2u1ffCtnFvi1qT599ArvIgTZ"
					 +"kgIBIjNTpCJ91iDJrkDehNGYrjGUf26162DxuoLoZBBOkxf33JdvmITQpksDWKw8"
					 +"zDKCXnX9HENHLC2tQC2hL8416yK8rr0F+XUpNZFDiR5yEUVJmQtDEIx+jDGoukTY"
					 +"70Kt6xf2XlgE/bT8GnEAq9rtUPDxJJixP+KDaUqzBKCD2M7c9A71PF1ozkqcUhIe"
					 +"JZdeWtwSDjae9T0Q8wd3+7rCj5BHQ8m4/S6904RK0HsqU7jplnrATQkTZZmO71FU"
					 +"6Otl8NwDicIOF1x4PQgXTDOD4j5EapljtvNcHTv5OiJrppXPY+51";
		String priv = "AAABAGhNyrQZgw0pRHVIo53qEynX676cbBKAhNN3QzvaGAg/FkIqGlS0jm2C+0KR"
					+"9nwhABt561SMWJH0HHGHYgiBMjmx+apL0iJIfWf5803SHsDGZ/14qAF7gSBCo871"
					+"Fvtdz10SugY12TQ3hDB8U4FZLQwX+EL8S1h8YdfEQhSlu5lcvdJdM00hbVylO/eD"
					+"Ym0kKcyrcamZtHgTxUFa/QCw1RmpUA/whpkeIHg6Z0iM+Wtihi97zsvMAq7irL74"
					+"8TqfsqhWnQys6LT0rS4ogq5JL+meBgAPOn0UhWd9VYwzazzhXP8fMESzidPhkh69"
					+"drEzVmXtL0EHjBfHEIwKxByn7xkAAACBAOEisHMFj5ozIdYmodUrgPG5XLrU73/C"
					+"8/0BKnRaj4Fi1hdnwwnXgMbltOdYmh1bYWTv55qXvRXRq8uMRkZtOj6NvzBBiHGF"
					+"8sYohlAQrTUe4JwqcRPdstklwDtCMYnMRMB4vvaFMm+n4VxioIeIcOSL0vBIfV/E"
					+"/d8xPi2PEqCTAAAAgQDge0ax5R5NTU43m1lvfV924E7ukWzAHZdR3Y68uWKh3sTl"
					+"EZ2IWnAEGCtfKZQxeDPDuafAyfRJugcKESydpoOVIoI5T9DHMiDtAy4InEH13aZV"
					+"HiXvVxMyUUux6t77veWapoLa4ee5i/Hx/qyJRoK4yDC+4R9TswlZm1qNEveB1wAA"
					+"AIEA4QKsjSBDWpbwnlDy2fQS/M/ohHvyCLkGCNn70vvuiuvscVtOQBtvYBOBU5nQ"
					+"aNKIfjunrPZ3Asc0Jc3zXOKt+iXULRWuW1NRDh6FX/X7zb0+groV/AgtWvh9Iup3"
					+"8GTl7Q35hubk/GSCKosY+wWuN0Qt34pxB/TpGHeF98W67bE=";
		priv_key = priv.getBytes();
		pub_key = pub.getBytes();

	}
*/
	public int fileSyncSetup(ArrayList<UserFile> filesToSync, String syncedDir, Institution inst){

		this.syncedDirPath = syncedDir;
		validateDestinationDirectory(inst.getName());
		glyphEdPath = syncedDirPath+inst.getName();

		filesToSync = setModifiedDates(filesToSync, inst);
		needToSync = new ArrayList<UserFile>();

		for(UserFile fileToSync : filesToSync){
			File f = new File(syncedDirPath+fileToSync.getFormattedPath().split(".zip")[0]);
			System.out.println(syncedDirPath+fileToSync.getFormattedPath().split(".zip")[0]);
			if(f.exists()){
				System.out.println(f.lastModified() + " and " + fileToSync.getLastModified().getTime());
				if(f.lastModified() < fileToSync.getLastModified().getTime()){
					needToSync.add(fileToSync);
					setNewModified(f, fileToSync.getLastModified().getTime());
				}
			}
			else{
				needToSync.add(fileToSync);
				setNewModified(f, fileToSync.getLastModified().getTime());
			}
		}

		File img = new File(syncedDirPath+inst.getName()+"/customer.png");
		if(img.exists()){
			if(img.lastModified() < inst.getLogoModified().getTime()){
				needToSyncImage = 1;
				needToSync.add(new UserFile("Logo", inst.getName()+"/customer.png", 1, 2));
				setNewModified(img, inst.getLogoModified().getTime());
			}
		}
		else{
			needToSyncImage = 1;
			needToSync.add(new UserFile("Logo", inst.getName()+"/customer.png", 1, 2));
			setNewModified(img, inst.getLogoModified().getTime());
		}
		File shr = new File(syncedDirPath+inst.getName()+"/sharedvisualizations.xml");
		if(shr.exists()){
			if(shr.lastModified() < inst.getSharedModified().getTime()){
				needToSyncShared = 1;
				needToSync.add(new UserFile("Shared", inst.getName()+"/sharedvisualizations.xml", 1, 4));
				setNewModified(shr, inst.getSharedModified().getTime());
			}
		}
		else{
			needToSyncShared = 1;
			needToSync.add(new UserFile("Shared", inst.getName()+"/sharedvisualizations.xml", 1, 4));
			setNewModified(shr, inst.getSharedModified().getTime());
		}
		File db = new File(syncedDirPath+inst.getName()+"/glyphed.db");
		if(db.exists()){
			if(db.lastModified() < inst.getDBModified().getTime()){
				needToSyncData = 1;
				needToSync.add(new UserFile("Data", inst.getName()+"/glyphed.zip", 1, 3));
				setNewModified(db, inst.getDBModified().getTime());
			}
		}
		else{
			needToSyncData = 1;
			needToSync.add(new UserFile("Data", inst.getName()+"/glyphed.zip", 1, 3));
			setNewModified(db, inst.getDBModified().getTime());
		}

		return needToSync.size();
	}

	public ArrayList<UserFile> setModifiedDates(ArrayList<UserFile> filesToSync, Institution inst){

		Vector list = null;
		ChannelSftp.LsEntry lsEntry = null;
    	SftpATTRS attrs = null;
		try{
			jsch=new JSch();
		    jsch.addIdentity(key);
		    //jsch.addIdentity("sgxinstancekey", priv_key, pub_key, new byte[0]);
		    session=jsch.getSession(USER, HOST, PORT);

		    Hashtable<String, String> config = new Hashtable<String, String>();
		    config.put("StrictHostKeyChecking", "no");
		    session.setConfig(config);
		    session.connect();

		    ChannelSftp chansftp = (ChannelSftp)session.openChannel("sftp");
		    chansftp.connect();

		    String rp = inst.getName()+"/";
		    list = chansftp.ls(rp);

	      	for (Object sftpFile : list) {
                lsEntry = (ChannelSftp.LsEntry) sftpFile;
                attrs = lsEntry.getAttrs();
                if(lsEntry.getFilename().equals("customer.png")){
                	inst.setLogoModified(convertTS(attrs.getMTime()));
                }
                else if(lsEntry.getFilename().equals("sharedvisualizations.xml")){
                	inst.setSharedModified(convertTS(attrs.getMTime()));
                }
                else if(lsEntry.getFilename().equals("glyphed.zip")){
                	inst.setDataModified(convertTS(attrs.getMTime()));
                }else{
                	for(UserFile fileToSync : filesToSync){
                		if(fileToSync.getRemotePath().equals(rp+lsEntry.getFilename())){
                			fileToSync.setLastModified(convertTS(attrs.getMTime()));
                		}
                	}
                }
            }
            chansftp.disconnect();

		}catch(Exception e){
			e.printStackTrace();
		}
		return filesToSync;

	}

	public int visualizationsToSync(){
		return ((needToSync.size() - needToSyncImage) - needToSyncData) - needToSyncShared;
	}

	public void startSyncingFiles(){

		Thread thread = new Thread(){
    		public void run(){
    			for(int i = 0; i < needToSync.size(); i++){
    				scpFrom(syncedDirPath+needToSync.get(i).getFormattedPath(), needToSync.get(i).getRemotePath());
    				try{
    					if(needToSync.get(i).getFileType() != 2 && needToSync.get(i).getFileType() != 4){
				    		UnzipUtility.unzip(syncedDirPath+needToSync.get(i).getFormattedPath(), syncedDirPath+needToSync.get(i).getFormattedPath().split("/")[0],needToSync.get(i).getFileType());
				    		UnzipUtility.delete(syncedDirPath+needToSync.get(i).getFormattedPath());
				    	}
			    	}catch(Exception e){e.printStackTrace();}
    				files_synced += 1;
    			}
    			updateTimestamps();
				session.disconnect();
				PathBuilder pb = new PathBuilder(glyphEdPath);
				if(visualizationsToSync() > 0){
					pb.updateSDTPathways();
				}
				pb.resetSharedVisualizationPaths();
    			doneSyncing = true;
    		}
  		};
  		thread.start();
	}

	public int filesSynced(){
		if(doneSyncing){
			session.disconnect();
		}
		return files_synced;
	}

	public void validateDestinationDirectory(String inst_name){

		String last = syncedDirPath.substring(syncedDirPath.length()-1);
		if(!(last.equals("/") || last.equals("\\"))){
			syncedDirPath += "/";
		}
		//System.out.println(syncedDirPath);
		key = syncedDirPath+"sgxinstancekey.ppk";

		File f = new File(syncedDirPath+inst_name);
		if(!f.exists()){
			f.mkdir();
		}
	}

	public void scpFrom(String lfile, String rfile){ //lfile = local, rfile = remote

		FileOutputStream fos=null;
	    try{

	      	String prefix=null;
	      	if(new File(lfile).isDirectory()){
	        	prefix=lfile+File.separator;
	      	}

	      	// exec 'scp -f rfile' remotely
	      	String command="scp -f \""+rfile+"\"";
	      	Channel channel=session.openChannel("exec");
	      	((ChannelExec)channel).setCommand(command);

	      	// get I/O streams for remote scp
	      	OutputStream out=channel.getOutputStream();
	      	InputStream in=channel.getInputStream();

	      	channel.connect();

	      	byte[] buf=new byte[1024];

	      	// send '\0'
	      	buf[0]=0; out.write(buf, 0, 1); out.flush();

	      	while(true){
				int c=checkAck(in);
	        	if(c!='C'){
		  			break;
				}

	        	// read '0644 '
	        	in.read(buf, 0, 5);

	        	long filesize=0L;
	        	while(true){
	          		if(in.read(buf, 0, 1)<0){
	            		// error
	            		break; 
	          		}
	          		if(buf[0]==' ')break;
	          		filesize=filesize*10L+(long)(buf[0]-'0');
	        	}

	        	String file=null;
	        	for(int i=0;;i++){
	          		in.read(buf, i, 1);
	          		if(buf[i]==(byte)0x0a){
	            		file=new String(buf, 0, i);
	            		break;
	  	  			}
        		}
				//System.out.println("filesize="+filesize+", file="+file);

        		// send '\0'
        		buf[0]=0; out.write(buf, 0, 1); out.flush();

        		// read a content of lfile
        		fos=new FileOutputStream(prefix==null ? lfile : prefix+file);
        		int foo;
        		while(true){
          			if(buf.length<filesize) foo=buf.length;
	  				else foo=(int)filesize;
          			foo=in.read(buf, 0, foo);
          			if(foo<0){
            			// error 
            			break;
          			}
          			fos.write(buf, 0, foo);
          			filesize-=foo;
          			if(filesize==0L) break;
        		}
        		fos.close();
        		fos=null;

				if(checkAck(in)!=0){
	  				System.exit(0);
				}
        		// send '\0'
        		buf[0]=0; out.write(buf, 0, 1); out.flush();
      		}

      		channel.disconnect();
      
	    }
	    catch(Exception e){
	      System.out.println(e);
	      try{if(fos!=null)fos.close();}catch(Exception ee){}
	    }
	}

	private static long convertTS(int ts){
		String ts_str = String.valueOf(ts);
		if(ts_str.length() < 13){
			int l = 13-ts_str.length();
			for(int i = 0; i < l; i++){
				ts_str += "0";
			}
		}
		return Long.parseLong(ts_str);
	}

	private static int checkAck(InputStream in) throws IOException{
	    int b=in.read();
	    // b may be 0 for success,
	    //          1 for error,
	    //          2 for fatal error,
	    //          -1
	    if(b==0) return b;
	    if(b==-1) return b;

	    if(b==1 || b==2){
	      	StringBuffer sb=new StringBuffer();
	      	int c;
	      	do {
				c=in.read();
				sb.append((char)c);
	      	}
	      	while(c!='\n');
	      	if(b==1){ // error
				System.out.print(sb.toString());
	      	}
	      	if(b==2){ // fatal error
				System.out.print(sb.toString());
	      	}
	    }
	    return b;
	}

	private void setNewModified(File f, long ts){
		if(toUpdateTS == null){
			toUpdateTS = new Hashtable<File, Long>();
		}
		toUpdateTS.put(f, ts);
	}

	private void updateTimestamps(){
		if(toUpdateTS != null){
			for(Map.Entry<File,Long> entry : toUpdateTS.entrySet()){
				entry.getKey().setLastModified(entry.getValue());
			}
		}
	}

	public String getGlyphEdPath(){
		return glyphEdPath;
	}

}