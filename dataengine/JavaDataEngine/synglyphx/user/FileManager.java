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
import java.io.File;
import synglyphx.util.ErrorHandler;

public class FileManager {

	private static String accessKey = "AKIAUWRBLENVICAQBC4Z";
	private static String secretKey = "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";
	private static AmazonS3 s3 = null;

	public FileManager() {
		s3 = new AmazonS3Client(new BasicAWSCredentials(accessKey, secretKey));
	}

	public boolean downloadFile(String bucket_name, String key, String out_location) {

		boolean success = false;

		TransferManager xfer_mgr = TransferManagerBuilder.standard().withS3Client(s3).build();

		try {
			File toAdd = new File(out_location, key.split("/")[1]);
			Download xfer = xfer_mgr.download(bucket_name, key, toAdd);
			xfer.waitForCompletion();
			success = true;
		}
		catch (Exception e) {
			try{
				e.printStackTrace();
			}
			catch(Exception eh){}
		}
		xfer_mgr.shutdownNow();

		return success;	
	}
}