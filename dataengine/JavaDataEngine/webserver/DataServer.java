// File Name DataServer.java
import java.net.*;
import java.io.*;
import org.json.*;
import synglyphx.io.DownloadedMap;

public class DataServer extends Thread {
   private ServerSocket serverSocket;
   
   public DataServer(int port) throws IOException {
      serverSocket = new ServerSocket(port);
      //serverSocket.setSoTimeout(60000);
   }

   public void run() {
      while(true) {
         try {
            System.out.println("Waiting for client on port " + 
               serverSocket.getLocalPort() + "...");
            Socket server = serverSocket.accept();
            
            System.out.println("Just connected to " + server.getRemoteSocketAddress());
            DataInputStream in = new DataInputStream(server.getInputStream());
            
            //System.out.println(in.readUTF());
            JSONObject obj = new JSONObject(in.readUTF());
            System.out.println("Output Dir: " + obj.get("output"));
            System.out.println("SDT Path: " + obj.get("sdt"));
            JSONArray queries = (JSONArray)obj.get("query");

            GlyphEngine glyphEngine = new GlyphEngine();
            double[] nw = new double[2];
            nw[0] = -180.0; nw[1] = 90.0;
            //nw[1] = 39.2141303; nw[0] = -83.4315742;
            double[] se = new double[2];
            se[0] = 180.0; se[1] = -90.0;
            //se[1] = 36.355921016; se[0] = -75.2755841;
            double[] s = new double[2];
            s[0] = 2048.0; s[1] = 1024.0;
            //s[0] = 1534.0; s[1] = 707.0;

            int err = glyphEngine.initiate((String)obj.get("sdt"), (String)obj.get("output"), "Web");

            for(int i = 0; i < queries.length(); i++){
               JSONArray list = (JSONArray)queries.get(i);
               System.out.println("ID: " + list.get(0));
               System.out.println("Table: " + list.get(1));
               System.out.println("Query: " + list.get(2));
               glyphEngine.setQueryForDatasource((String)list.get(0), (String)list.get(1), (String)list.get(2));
            }

            if(glyphEngine.isUpdateNeeded()){
               if(glyphEngine.hasDMBaseObject()){
                  double[] nwse = glyphEngine.getNWandSE();
                  System.out.println("Initial Northwest Corner: "+String.valueOf(nwse[0])+", "+String.valueOf(nwse[1]));
                  System.out.println("Initial Southeast Corner: "+String.valueOf(nwse[2])+", "+String.valueOf(nwse[3]));
                  DownloadedMap dm = new DownloadedMap(glyphEngine.getDMBaseObject(), nwse);
                  dm.downloadMap((String)obj.get("output"));
                  nw = dm.northWestCorner();
                  se = dm.southEastCorner();
                  s = dm.imageSize();
                  System.out.println("Northwest Corner: "+String.valueOf(nw[0])+", "+String.valueOf(nw[1]));
                  System.out.println("Southeast Corner: "+String.valueOf(se[0])+", "+String.valueOf(se[1]));
                  System.out.println("Image Size: "+String.valueOf(s[0])+", "+String.valueOf(s[1]));
               }
               glyphEngine.setBoundingBox(nw,se,s);
               err = glyphEngine.beginGlyphGeneration();
            }

            DataOutputStream out = new DataOutputStream(server.getOutputStream());
            out.writeUTF("Thank you for connecting to " + server.getLocalSocketAddress()
               + "\nGoodbye!");
            server.close();
            
         }
         catch(SocketTimeoutException s) {
            System.out.println("Socket timed out!");
            break;
         }
         catch(JSONException j) {
            j.printStackTrace();
            break;
         }
         catch(IOException e) {
            e.printStackTrace();
            break;
         }
      }
   }
   
   public static void main(String [] args) {
      int port = Integer.parseInt(args[0]);
      try {
         Thread t = new DataServer(port);
         t.start();
      }catch(IOException e) {
         e.printStackTrace();
      }
   }
}