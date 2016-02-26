import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.esri.toolkit.overlays.InfoPopupOverlay;
import com.esri.toolkit.utilities.ResultPanel;
import com.esri.core.geometry.Envelope;
import com.esri.core.geometry.Point;
import com.esri.core.map.CallbackListener;
import com.esri.core.map.Graphic;
import com.esri.core.portal.Portal;
import com.esri.core.symbol.PictureMarkerSymbol;
import com.esri.core.tasks.geocode.Locator;
import com.esri.core.tasks.geocode.LocatorFindParameters;
import com.esri.core.tasks.geocode.LocatorGeocodeResult;
import com.esri.core.io.UserCredentials;
import com.esri.map.ArcGISTiledMapServiceLayer;
import com.esri.map.GraphicsLayer;
import com.esri.map.JMap;
import com.esri.map.LayerInitializeCompleteEvent;
import com.esri.map.LayerInitializeCompleteListener;
import com.esri.map.LayerList;


public class TestArcGIS {
	
  	private Locator locator;
    private Portal portal;
  	private double start;
  	private double end;
  	private double total;
    private String CLIENT_SECRET = "13f289f342cd422ebd51395d27d05230";
    private String CLIENT_ID = "9JpposIsCzwIqMeE";

  	public TestArcGIS(){}

    public void processAddress(List<Map<String,String>> addresses, LocatorFindParameters params) {
        //set parameters including address selected in combo
        start = System.currentTimeMillis();

        //uc.setUserAccount("Holster", "bh108381");
        //uc.setUserToken("Sn4HSztTQdx2wJv_4-P_4iClyDZ4SaDzO4d53TtzV2mH9LYIs2rejpzCE3T0C8zOEBAjRHLFDMu6052ZgcA70YfEyydFr4kEBzuVV0E8oVUQ2pw9a8y9_PyiJ7SfzsjqJWjc4LmfNdkld49tkwtD1w..","Glyph Builder");
        
        //String url = "http://geoenrich.arcgis.com/arcgis/rest/services/World/GeoenrichmentServer/Geoenrichment/enrich";
        //url += "?token=9wS01CNABIa-X4ot0MyDu2j3iKPQVnf4jA8KgM-iTwxFi38t9ewWkcsQ5mDNULmOGWpsr_aaNtmMZ2xZxWdXCZS5QpkVG30cxXUiNAYgVAxXqudf-Mbxk5k3cwDTOBOskiE_-Zx9hWxZpzZJ64Q_4Q..";
        portal = new Portal("http://sgx.maps.arcgis.com/",null);

        try{
          portal.doOAuthAppAuthenticate(CLIENT_SECRET, CLIENT_ID, new CallbackListener<Portal>() {
            @Override
            public void onError(Throwable e) {
              // handle the error, alert the user
              System.out.println("failed to signed in!");
            }
           
            @Override
            public void onCallback(Portal objs) {
              // take some action upon successful authentication
              System.out.println("app signed in!");
            }
          });
        }catch(Exception e){
          e.printStackTrace();
        }

        
        //locator = Locator.createOnlineLocator(
        //"https://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer/geocodeAddresses");
        //"http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer");

        //run the locator find task asynchronously to not freeze the UI
        try{
	        //displayResults(locator.batchGeocode(addresses, params.getOutSR()).getLocations());
	        //end = System.currentTimeMillis();
	        //total += end-start;
	        //System.out.println(end-start);
	    }catch(Exception e){
	    	e.printStackTrace();
	    }
    }

    public void printTotal(){
    	System.out.println(total);
    }

    private void displayResults(List<LocatorGeocodeResult> results) {

	    // get the top result to display on map
      for(int i = 0; i < results.size(); i++){
        LocatorGeocodeResult result = results.get(i);
        Point coords = result.getLocation();
        System.out.println(i);
        System.out.println("Address: "+result.getAddress());
        System.out.println("Coordinates: "+String.valueOf(coords.getX())+", "+String.valueOf(coords.getY()));
      }

    }

    public static void main(String[] args) {
    	TestArcGIS gis = new TestArcGIS();

      LocatorFindParameters params = new LocatorFindParameters("4181 Cray Dr. Warrenton, VA 20187");
      params.setMaxLocations(1);

      List<Map<String,String>> addresses = new ArrayList<Map<String,String>>();
      for(int i = 0; i < 100; i++){
        addresses.add(new HashMap<String,String>());
        addresses.get(i).put("Address", "4181 Cray Dr.");
        addresses.get(i).put("City", "Warrenton");
        addresses.get(i).put("Region", "VA");
        addresses.get(i).put("Postal", "20187");
      }

      gis.processAddress(addresses, params);
      gis.printTotal();

  	}
}