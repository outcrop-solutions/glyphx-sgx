package synglyphx.user;

import com.jcraft.jsch.*;
import java.io.*;
import java.util.Hashtable;
import java.util.ArrayList;

public class FileSyncer {

	private static String KEY = "C:/Users/Bryan/Desktop/GlyphEd/sgxinstancekey.ppk";
	private static String HOST = "ec2-52-42-56-15.us-west-2.compute.amazonaws.com";
	private static String USER = "ec2-user";
	private static int PORT = 22;
	private ArrayList<UserFile> needToSync = null;
	private String syncedDirPath;
	private JSch jsch = null;
	private Session session = null;
	private int needToSyncImage;
	private int needToSyncData;
	private volatile int files_synced;

	public FileSyncer(){
		needToSyncImage = 0;
		needToSyncData = 0;
		files_synced = 1;
	}
	
	public int fileSyncSetup(ArrayList<UserFile> filesToSync, String syncedDirPath, Institution inst){

		this.syncedDirPath = syncedDirPath;
		needToSync = new ArrayList<UserFile>();

		validateDestinationDirectory(inst.getName());

		for(UserFile fileToSync : filesToSync){
			File f = new File(syncedDirPath+fileToSync.getFormattedPath().split(".zip")[0]);
			if(f.exists()){
				if(f.lastModified() < fileToSync.getLastModified().getTime()){
					needToSync.add(fileToSync);
				}
			}
			else{
				needToSync.add(fileToSync);
			}
		}

		File img = new File(syncedDirPath+inst.getName()+"/customer.png");
		if(img.exists()){
			if(img.lastModified() < inst.getLogoModified().getTime()){
				needToSyncImage = 1;
				needToSync.add(new UserFile("Logo", inst.getName().replace(" ","_")+"/customer.png", 1, inst.getLogoModified(), 2));
			}
		}
		else{
			needToSyncImage = 1;
			needToSync.add(new UserFile("Logo", inst.getName().replace(" ","_")+"/customer.png", 1, inst.getLogoModified(), 2));
		}
		File db = new File(syncedDirPath+inst.getName()+"/glyphed.db");
		if(db.exists()){
			if(db.lastModified() < inst.getDBModified().getTime()){
				needToSyncData = 1;
				needToSync.add(new UserFile("Data", inst.getName().replace(" ","_")+"/glyphed.zip", 1, inst.getDBModified(), 3));
			}
		}
		else{
			needToSyncData = 1;
			needToSync.add(new UserFile("Data", inst.getName().replace(" ","_")+"/glyphed.zip", 1, inst.getDBModified(), 3));
		}

		return needToSync.size();
	}

	public int visualizationsToSync(){
		return (needToSync.size() - needToSyncImage) - needToSyncData;
	}

	public void startSyncingFiles(){

		try{
			jsch=new JSch();
		    jsch.addIdentity(KEY);
		    session=jsch.getSession(USER, HOST, PORT);

		    Hashtable<String, String> config = new Hashtable<String, String>();
		    config.put("StrictHostKeyChecking", "no");
		    session.setConfig(config);
		    session.connect();
		}catch(Exception e){
			e.printStackTrace();
		}

		Thread thread = new Thread(){
    		public void run(){
    			for(int i = 0; i < needToSync.size(); i++){
    				scpFrom(syncedDirPath+needToSync.get(i).getFormattedPath(), needToSync.get(i).getRemotePath());
    				try{
    					if(needToSync.get(i).getFileType() != 2){
				    		UnzipUtility.unzip(syncedDirPath+needToSync.get(i).getFormattedPath(), syncedDirPath+needToSync.get(i).getFormattedPath().split("/")[0],needToSync.get(i).getFileType());
				    		UnzipUtility.delete(syncedDirPath+needToSync.get(i).getFormattedPath());
				    	}
			    	}catch(Exception e){e.printStackTrace();}
    				files_synced += 1;
    			}
    		}
  		};
  		thread.start();
	}

	public int filesSynced(){
		if(files_synced == (needToSync.size()+1)){
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
	      	String command="scp -f "+rfile;
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

}