// File Name DataClient.java
import java.net.*;
import java.io.*;

public class DataClient {

   public static void main(String [] args) {
      String serverName = args[0];
      int port = Integer.parseInt(args[1]);
      try {
         System.out.println("Connecting to " + serverName + " on port " + port);
         Socket client = new Socket(serverName, port);
         
         System.out.println("Just connected to " + client.getRemoteSocketAddress());
         OutputStream outToServer = client.getOutputStream();
         DataOutputStream out = new DataOutputStream(outToServer);
         /*
         String runStr = "{\"output\": \"D:/Users/Bryan/Desktop/WebEngine/Demo/Independent\", " +
         "\"sdt\": \"C:/ProgramData/SynGlyphX/Content/glyphed_demo/Global Admissions/Global Admissions.sdt\", " +
         "\"query\": [\"719598f5-ff35-47b4-bf7a-9bdfbb30140f\",\"GlobalAdmissions\",\"SELECT * FROM GlobalAdmissions WHERE StaffAssigned IN ('Staff 1') AND Year IN ('2017')\"]}";
         */
         /*
         String runStr = "{\"output\": \"D:/Users/Bryan/Desktop/WebEngine/Demo/Independent\", " +
         "\"sdt\": \"C:/ProgramData/SynGlyphX/Content/glyphed_demo/Admissions Officer/Applicants.sdt\", " +
         "\"query\": [\"0e10b5e1-60fb-4fbc-8e53-0988839dc495\",\"GlobalAdmissions\",\"SELECT * FROM GlobalAdmissions WHERE StaffAssigned IN ('Staff 1') AND Year IN ('2017')\"]}";
        */
         /*String runStr = "{\"output\": \"D:/Users/Bryan/Desktop/WebEngine/Demo/Independent\", " +
         "\"sdt\": \"C:/ProgramData/SynGlyphX/Content/glyphed_demo/Prospect and Segmentation/Prospects Only.sdt\", " +
         "\"query\": [\"1663ee8e-e7f8-445c-963b-79e8f45d8ee8\",\"ProspectsOnlyView\",\"SELECT * FROM ProspectsOnlyView WHERE HSstate IN ('TX')\"]}";*/
         String runStr = "{\"output\":\"D:/Users/Bryan/Desktop/WebEngine/Demo/Independent\", " +
         "\"sdt\":\"C:/ProgramData/SynGlyphX/Content/gannon/Retention/Retention Non Geo.sdt\", " +
         "\"query\":[\"9502b3d4-6376-42fc-a406-7077357e8eca\",\"RetentionNonGeo\",\"SELECT * FROM RetentionNonGeo WHERE PermState IN ('AL');\"]}";
         /*"\"query\":[[\"1663ee8e-e7f8-445c-963b-79e8f45d8ee8\",\"HS_Prospect_View\",\"SELECT * FROM HS_Prospect_View WHERE hs_geo_market IN ('VA-06 Richmond');\"], " +
         "[\"1663ee8e-e7f8-445c-963b-79e8f45d8ee8\",\"ProspectsOnlyView\",\"SELECT * FROM ProspectsOnlyView WHERE HSGeomarket IN ('VA-06 Richmond');\"]]}";*/
         //"\"query\":[\"1663ee8e-e7f8-445c-963b-79e8f45d8ee8\",\"HS_Prospect_View\",\"SELECT * FROM HS_Prospect_View WHERE HSName IN ('Paul VI Catholic High School');\"]}";
         out.writeUTF(runStr);
         InputStream inFromServer = client.getInputStream();
         DataInputStream in = new DataInputStream(inFromServer);
         
         System.out.println("Server says " + in.readUTF());
         client.close();
      }catch(IOException e) {
         e.printStackTrace();
      }
   }
}