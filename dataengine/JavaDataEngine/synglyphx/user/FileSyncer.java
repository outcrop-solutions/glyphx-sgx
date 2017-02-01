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

	private static String HOST = "ec2-52-42-56-15.us-west-2.compute.amazonaws.com";
	private static String USER = "ec2-user";
	private static int PORT = 22;
	private File ppk = null;
	private ArrayList<UserFile> userFiles = null;
	private ArrayList<UserFile> needToSync = null;
	private Hashtable<File, Long> toUpdateTS = null;
	private String syncedDirPath;
	private String glyphEdPath;
	private JSch jsch = null;
	private Session session = null;
	private int needToSyncImage;
	private int needToSyncData;
	private int needToSyncShared;
	private volatile boolean doneSyncing;
	private volatile int files_synced;

	public FileSyncer(){
		needToSyncImage = 0;
		needToSyncShared = 0;
		needToSyncData = 0;
		files_synced = 1;
		doneSyncing = false;
	}

	private void setupPrivateKey(){
		
		String ppkey = "-----BEGIN RSA PRIVATE KEY-----" + System.lineSeparator()
		+"MIIEpAIBAAKCAQEAxWrEO1rbc9rtX3wrZxb4tak+ffQK7yIE2ZICASIzU6QifdYg" + System.lineSeparator()
		+"ya5A3oTRmK4xlH9utetg8bqC6GQQTpMX99yXb5iE0KZLA1isPMwygl51/RxDRywt" + System.lineSeparator()
		+"rUAtoS/ONesivK69Bfl1KTWRQ4kechFFSZkLQxCMfowxqLpE2O9CresX9l5YBP20" + System.lineSeparator()
		+"/BpxAKva7VDw8SSYsT/ig2lKswSgg9jO3PQO9TxdaM5KnFISHiWXXlrcEg42nvU9" + System.lineSeparator()
		+"EPMHd/u6wo+QR0PJuP0uvdOEStB7KlO46ZZ6wE0JE2WZju9RVOjrZfDcA4nCDhdc" + System.lineSeparator()
		+"eD0IF0wzg+I+RGqZY7bzXB07+Toia6aVz2PudQIDAQABAoIBAGhNyrQZgw0pRHVI" + System.lineSeparator()
		+"o53qEynX676cbBKAhNN3QzvaGAg/FkIqGlS0jm2C+0KR9nwhABt561SMWJH0HHGH" + System.lineSeparator()
		+"YgiBMjmx+apL0iJIfWf5803SHsDGZ/14qAF7gSBCo871Fvtdz10SugY12TQ3hDB8" + System.lineSeparator()
		+"U4FZLQwX+EL8S1h8YdfEQhSlu5lcvdJdM00hbVylO/eDYm0kKcyrcamZtHgTxUFa" + System.lineSeparator()
		+"/QCw1RmpUA/whpkeIHg6Z0iM+Wtihi97zsvMAq7irL748TqfsqhWnQys6LT0rS4o" + System.lineSeparator()
		+"gq5JL+meBgAPOn0UhWd9VYwzazzhXP8fMESzidPhkh69drEzVmXtL0EHjBfHEIwK" + System.lineSeparator()
		+"xByn7xkCgYEA4SKwcwWPmjMh1iah1SuA8blcutTvf8Lz/QEqdFqPgWLWF2fDCdeA" + System.lineSeparator()
		+"xuW051iaHVthZO/nmpe9FdGry4xGRm06Po2/MEGIcYXyxiiGUBCtNR7gnCpxE92y" + System.lineSeparator()
		+"2SXAO0IxicxEwHi+9oUyb6fhXGKgh4hw5IvS8Eh9X8T93zE+LY8SoJMCgYEA4HtG" + System.lineSeparator()
		+"seUeTU1ON5tZb31fduBO7pFswB2XUd2OvLliod7E5RGdiFpwBBgrXymUMXgzw7mn" + System.lineSeparator()
		+"wMn0SboHChEsnaaDlSKCOU/QxzIg7QMuCJxB9d2mVR4l71cTMlFLsere+73lmqaC" + System.lineSeparator()
		+"2uHnuYvx8f6siUaCuMgwvuEfU7MJWZtajRL3gdcCgYBeMOA3zzzMW9YtPY55nDDf" + System.lineSeparator()
		+"cRjdxdbJ0iu4T3OOqJDfphVCR5QvGu1yyYoNPGGGJMQzsbdpU1C1vzor9+0y6+Hx" + System.lineSeparator()
		+"Z+Z0bwaB0HPI+g1vk/qTfe6lB5C1qgx4kzXA1SnaqA6fpCTpCMMYOdmsiEr3SVtK" + System.lineSeparator()
		+"E0HWa82gcs9wQK7t6qz1ZwKBgQCgvaVRnQ6lq8VmbAcOnmfNZ7Edg2Sr7IR+Snf7" + System.lineSeparator()
		+"C4RlBG9Y19RF3vx28hecfNlw0vG+JSLKGdKjOGHyA3o/KQzvrVYyU8Tb3jlVfgFo" + System.lineSeparator()
		+"iPtyYtTH/l6Lk35beXsKW3RwLvpKUxFASqLzahJFhmdlpOfWCQNeqzUTahA36dZd" + System.lineSeparator()
		+"TkeM4QKBgQDhAqyNIENalvCeUPLZ9BL8z+iEe/IIuQYI2fvS++6K6+xxW05AG29g" + System.lineSeparator()
		+"E4FTmdBo0oh+O6es9ncCxzQlzfNc4q36JdQtFa5bU1EOHoVf9fvNvT6CuhX8CC1a" + System.lineSeparator()
		+"+H0i6nfwZOXtDfmG5uT8ZIIqixj7Ba43RC3finEH9OkYd4X3xbrtsQ==" + System.lineSeparator()
		+"-----END RSA PRIVATE KEY-----" + System.lineSeparator();

		try{
			ppk = File.createTempFile("sgxinstancekey",".ppk");
			FileOutputStream fos = new FileOutputStream(ppk.getAbsolutePath());
			fos.write(ppkey.getBytes());
			fos.close();
		}
		catch(Exception e){
			e.printStackTrace();
		}

	}

	public int fileSyncSetup(ArrayList<UserFile> filesToSync, String syncedDir, Institution inst){

		this.syncedDirPath = syncedDir;
		validateDestinationDirectory(inst.getName());
		setupPrivateKey();
		glyphEdPath = syncedDirPath+inst.getName();

		filesToSync = setModifiedDates(filesToSync, inst);
		userFiles = filesToSync;
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

		File shr = new File(syncedDirPath+inst.getName()+"/syncedvisualizations.xml");
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

		if(needToSync.size() == 0){
			PathBuilder pb = new PathBuilder(glyphEdPath);
			pb.resetSharedVisualizationPaths(userFiles);
		}

		return needToSync.size();
	}

	public ArrayList<UserFile> setModifiedDates(ArrayList<UserFile> filesToSync, Institution inst){

		Vector list = null;
		ChannelSftp.LsEntry lsEntry = null;
    	SftpATTRS attrs = null;
		try{
			jsch=new JSch();
		    jsch.addIdentity(ppk.getAbsolutePath());
		    session=jsch.getSession(USER, HOST, PORT);
		    ppk.delete();

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
    						UnzipUtility.delete(syncedDirPath+needToSync.get(i).getFormattedPath().split(".zip")[0]);
				    		UnzipUtility.unzip(syncedDirPath+needToSync.get(i).getFormattedPath(), syncedDirPath+needToSync.get(i).getFormattedPath().split("/")[0],needToSync.get(i).getFileType());
				    		UnzipUtility.delete(syncedDirPath+needToSync.get(i).getFormattedPath());
				    	}
			    	}catch(Exception e){e.printStackTrace();}
			    	if(i == needToSync.size()-1){
			    		updateTimestamps();
						PathBuilder pb = new PathBuilder(glyphEdPath);
						if(visualizationsToSync() > 0){
							pb.updateSDTPathways();
						}
						pb.resetSharedVisualizationPaths(userFiles);
			    	}
    				files_synced += 1;
    			}
    			session.disconnect();
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