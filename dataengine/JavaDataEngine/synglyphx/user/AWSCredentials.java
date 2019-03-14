package synglyphx.user;

public class AWSCredentials {

    private static AWSCredentials instance;
    private String aws_s3_access_key;
    private String aws_s3_secret_access_key;
    private String aws_s3_bucket_name;
    private String aws_rds_host;
    private String aws_rds_user;
    private String aws_rds_pass;
    private String aws_rds_dbname;
    private String aws_rds_port;
    
    private AWSCredentials(){}
    
    public static AWSCredentials getInstance(){
        if(instance == null){
            instance = new AWSCredentials();
        }
        return instance;
    }

    public void setS3Credentials(String access_key, String secret_access_key, String bucket_name) {
    	this.aws_s3_access_key = access_key;
    	this.aws_s3_secret_access_key = secret_access_key;
    	this.aws_s3_bucket_name = bucket_name;
    }

    public void setRDSCredentials(String host, String user, String pass, String dbname, String port){
    	this.aws_rds_host = host;
    	this.aws_rds_user = user;
    	this.aws_rds_pass = pass;
    	this.aws_rds_dbname = dbname;
    	this.aws_rds_port = port;
    }

    public String getS3AccessKey(){
    	return aws_s3_access_key;
    }

    public String getS3SecretAccessKey(){
    	return aws_s3_secret_access_key;
    }

    public String getS3BucketName(){
    	return aws_s3_bucket_name;
    }

    public String getRDSHost(){
    	return aws_rds_host;
    }

    public String getRDSUser(){
    	return aws_rds_user;
    }

    public String getRDSPass(){
    	return aws_rds_pass;
    }

    public String getRDSDbname(){
    	return aws_rds_dbname;
    }

    public String getRDSPort(){
    	return aws_rds_port;
    }
}
