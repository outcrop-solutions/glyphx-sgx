package synglyphx.user;

import java.io.BufferedReader;
import java.io.OutputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;

import org.json.simple.JSONArray; 
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import javax.net.ssl.HttpsURLConnection;

import synglyphx.io.Logger;

public class SGXAuthAPI {

    // HTTPS POST request
    public static String sendPost(String user, String pass, String version) {

        try {
            String url = "https://ftczfcxjeb.execute-api.us-east-1.amazonaws.com/beta/auth";
            URL obj = new URL(url);
            HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

            //add reuqest header
            con.setDoOutput(true);
            con.setRequestMethod("POST");
            con.setRequestProperty("User-Agent", "Java client");
            con.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");

            String urlParameters = "{\"username\":\""+user+"\",\"password\":\""+pass+"\",\"apiVersion\":\""+version+"\"}";

            // Send post request
            OutputStream wr = con.getOutputStream();
            wr.write(urlParameters.getBytes("UTF-8"));
            wr.flush();
            wr.close();

            int responseCode = con.getResponseCode();
            //System.out.println("\nSending 'POST' request to URL : " + url);
            //System.out.println("Post parameters : " + urlParameters);
            //System.out.println("Response Code : " + responseCode);

            BufferedReader in = new BufferedReader(
                    new InputStreamReader(con.getInputStream()));
            String inputLine;
            StringBuffer response = new StringBuffer();

            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();

            //print result
            return response.toString();

        }catch(Exception e){
            try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
            e.printStackTrace();
        }

        return "{\"result\":\"{\"valid\":false}\"}";
    }
    
    public static HashMap<String, String> parseJson(String json) {

        JSONParser parser = new JSONParser();
        HashMap<String, String> user_data = new HashMap<String, String>();

        try {

            JSONObject jsonObject = (JSONObject) parser.parse(json);

            String result = (String) jsonObject.get("result");
            JSONObject resultObject = (JSONObject) parser.parse(result);

            if((boolean)resultObject.get("valid")) {
                String aws_s3_access_key = (String)resultObject.get("aws_s3_access_key");
                String aws_s3_secret_access_key = (String)resultObject.get("aws_s3_secret_access_key");
                String bucket_name = (String)resultObject.get("bucket_name");
                //System.out.println("S3 Access: "+aws_s3_access_key+"\nS3 Secret: "+aws_s3_secret_access_key+"\nS3 Bucket: "+bucket_name+"\n");
                String aws_rds_host = (String)resultObject.get("aws_rds_host");
                String aws_rds_user = (String)resultObject.get("aws_rds_user");
                String aws_rds_pass = (String)resultObject.get("aws_rds_pass");
                String aws_rds_dbname = (String)resultObject.get("aws_rds_dbname");
                String aws_rds_port = (String)resultObject.get("aws_rds_port");
                //System.out.println("RDS Host: "+aws_rds_host+"\nRDS User: "+aws_rds_user+"\nRDS Pass: "+aws_rds_pass+"\nRDS Dbname: "+aws_rds_dbname+"\nRDS Port: "+aws_rds_port+"\n");
                user_data.put("user_id", (String)resultObject.get("user_id"));
                user_data.put("user_name", (String)resultObject.get("user_name"));
                user_data.put("last_login", (String)resultObject.get("last_login"));
                user_data.put("inst_id", (String)resultObject.get("inst_id"));
                user_data.put("inst_name", (String)resultObject.get("inst_name"));
                //System.out.println("User ID: "+String.valueOf(user_id)+"\nUsername: "+user_name+"\nLast Login: "+last_login+"\nInst ID: "+String.valueOf(inst_id)+"\nInst Name: "+inst_name+"\n");
                AWSCredentials cred = AWSCredentials.getInstance();
                cred.setS3Credentials(aws_s3_access_key, aws_s3_secret_access_key, bucket_name);
                cred.setRDSCredentials(aws_rds_host, aws_rds_user, aws_rds_pass, aws_rds_dbname, aws_rds_port);
            }

        } catch (ParseException e) {
            try{
                e.printStackTrace(Logger.getInstance().addError());
            }catch(Exception ex){}
            e.printStackTrace();
        }

        return user_data;
    }
    /*
    public static void main(String[] args) throws Exception {

        SGXAuthAPI https = new SGXAuthAPI();

        String username = "bholster";
        String password = "bholster";
        String appVersion = "1.2.02";

        String response = https.sendPost(username, password, appVersion);
        System.out.println(response + "\n");

        https.parseJson(response);
    }
    */
}
