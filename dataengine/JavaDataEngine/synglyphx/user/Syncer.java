package synglyphx.user;

import com.amazonaws.AmazonServiceException;
import com.amazonaws.services.s3.transfer.TransferManager;
import com.amazonaws.services.s3.transfer.TransferManagerBuilder;
import com.amazonaws.services.s3.transfer.Download;
import com.amazonaws.services.s3.transfer.TransferProgress;
import com.amazonaws.auth.BasicAWSCredentials;
import com.amazonaws.services.s3.model.GetObjectRequest;
import com.amazonaws.services.s3.model.S3Object;
import com.amazonaws.services.s3.AmazonS3;
import com.amazonaws.services.s3.AmazonS3Client;
import com.amazonaws.services.s3.model.Bucket;
import com.amazonaws.services.s3.model.S3ObjectSummary;
import java.util.Arrays; 
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.io.File;
import java.util.Date;
import synglyphx.user.UnzipUtility;
import synglyphx.io.Logger;

public class Syncer {

	private static AmazonS3 s3 = null;
	private ArrayList<String> files_to_sync = null;
	private HashMap<String,Long> server_timestamps = null;
	private ArrayList<UserFile> userFiles = null;
	private String key_prefix;
	private String dir_path;
	private String bucket_name;
	private long bytes_to_sync;
	private int visualizations_to_sync;
	private volatile long bytes_synced;
	private volatile boolean complete;

	public Syncer(String key, String dir, ArrayList<UserFile> ufs) {
		
		AWSCredentials cred = AWSCredentials.getInstance();
		s3 = new AmazonS3Client(new BasicAWSCredentials(cred.getS3AccessKey(), cred.getS3SecretAccessKey()));
		this.bucket_name = "viz-group-"+key.replace("_", "-");
		this.key_prefix = key;
		this.dir_path = dir;
		userFiles = ufs;
		bytes_to_sync = 0;
		visualizations_to_sync = 0;
		bytes_synced = 0;
		complete = false;
	}

	public boolean isSyncingNeeded(ArrayList<String> filesToSync){

		for(int i = 0; i < filesToSync.size(); i++){
			Logger.getInstance().add(filesToSync.get(i));
		}
		files_to_sync = new ArrayList<String>();
		server_timestamps = new HashMap<String,Long>();
		new File(dir_path).mkdir();
		new File(dir_path, key_prefix).mkdir();
		List<S3ObjectSummary> objs = s3.listObjects(bucket_name, key_prefix).getObjectSummaries();
		for(S3ObjectSummary o : objs) {
			if(filesToSync.contains(o.getKey()) || !getExtension(o.getKey()).equals("zip") || o.getKey().contains("data.zip")){
				File dl = findCorrespondingFile(dir_path, o.getKey());
				if(dl.exists()){
					if(dl.lastModified() < o.getLastModified().getTime()){
						addFileToSync(o, filesToSync);
					}
				}
				else{
					addFileToSync(o, filesToSync);
				}
			}
		}
		if(files_to_sync.size() == 0){
			PathBuilder pb = new PathBuilder(getFinalDirectory());
			pb.resetSharedVisualizationPaths(userFiles);
			bytes_to_sync = 1;
			bytes_synced = 1;
			complete = true;
		}
		return files_to_sync.size() > 0 ? true : false;
	}

	private File findCorrespondingFile(String dir_path, String file_key){
		
		if(new File(dir_path, file_key.split(".zip")[0]).exists()){
			return new File(dir_path, file_key.split(".zip")[0]);
		}
		else if(new File(dir_path, file_key.split(".zip")[0]+".db").exists()){
			return new File(dir_path, file_key.split(".zip")[0]+".db");
		}
		else{
			return new File(dir_path, file_key);
		}
	}

	private void addFileToSync(S3ObjectSummary obj, ArrayList<String> filesToSync){
		files_to_sync.add(obj.getKey());
		bytes_to_sync += obj.getSize();
		server_timestamps.put(obj.getKey(), obj.getLastModified().getTime());
		if(filesToSync.contains(obj.getKey())){
			visualizations_to_sync += 1;
		}
	}

	private String getExtension(String file){
		String[] splt = file.split(".");
		return splt.length >= 1 ? splt[splt.length-1] : "";
	}

	public String getFinalDirectory(){
		return new File(dir_path, key_prefix).getAbsolutePath();
	}

	public int visualizationsToSync(){
		return visualizations_to_sync;
	}

	public void downloadFiles(){

		Logger.getInstance().add("Directory path: "+dir_path);
		Logger.getInstance().add("Key prefix: "+key_prefix);
		for(int i = 0; i < files_to_sync.size(); i++){
			Logger.getInstance().add(files_to_sync.get(i));
		}

		Thread thread = new Thread(){
    		public void run(){
    			TransferManager xfer_mgr = TransferManagerBuilder.standard().withS3Client(s3).build();
				try {
					for(int i = 0; i < files_to_sync.size(); i++){
						File toAdd = new File(dir_path,files_to_sync.get(i));
						Download xfer = xfer_mgr.download(
					            bucket_name, files_to_sync.get(i), toAdd);
					    long last_amount = 0;
					    do {
						    try {
						        Thread.sleep(200);
						    } catch (InterruptedException e) {}

						    TransferProgress progress = xfer.getProgress();
						    long xfered = progress.getBytesTransferred();
						    bytes_synced += xfered-last_amount;
						    last_amount = xfered;

						} while (xfer.isDone() == false);
						if(toAdd.getName().contains(".zip")){
							File nf = new File(dir_path+File.separator+key_prefix, UnzipUtility.unzip(toAdd.getAbsolutePath(), toAdd.getParent()));
							toAdd.delete();
							toAdd = nf;
						}
						toAdd.setLastModified(server_timestamps.get(files_to_sync.get(i)));
					}
					PathBuilder pb = new PathBuilder(getFinalDirectory());
					if(visualizationsToSync() > 0){
						pb.updateSDTPathways();
					}
					pb.resetSharedVisualizationPaths(userFiles);
					complete = true;

				} catch (Exception e) {
					e.printStackTrace();
				    System.exit(1);
				}
				xfer_mgr.shutdownNow();
    		}
    	};
    	thread.start();
	}

	public long getProgress(){
		long prog = bytes_synced/(bytes_to_sync/100);
		return prog > 100 ? 100 : prog;
	}

	public boolean isDone(){
		return (bytes_synced/bytes_to_sync)*100 == 100 && complete ? true : false;
	}
/*
	public static void main(String [] args){

		Syncer syncer = new Syncer("notredame", "C:/Users/Bryan/Documents/SyncedGlyphEdFiles");
		if(syncer.isSyncingNeeded()){
			syncer.downloadFiles();
			do{
				try {
			        Thread.sleep(500);
			    } catch (InterruptedException e) {}
			    System.out.println(syncer.getProgress());
			}while(syncer.isDone() == false);
		}
	}
*/
}