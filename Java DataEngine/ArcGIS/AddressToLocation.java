/* Copyright 2014 Esri

All rights reserved under the copyright laws of the United States
and applicable international laws, treaties, and conventions.

You may freely redistribute and use this sample code, with or
without modification, provided you include the original copyright
notice and use restrictions.

See the use restrictions.*/

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

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;

import com.esri.toolkit.overlays.InfoPopupOverlay;
import com.esri.toolkit.utilities.ResultPanel;
import com.esri.core.geometry.Envelope;
import com.esri.core.geometry.Point;
import com.esri.core.map.CallbackListener;
import com.esri.core.map.Graphic;
import com.esri.core.portal.Portal;
import com.esri.core.io.UserCredentials;
import com.esri.core.symbol.PictureMarkerSymbol;
import com.esri.core.tasks.geocode.Locator;
import com.esri.core.tasks.geocode.LocatorFindParameters;
import com.esri.core.tasks.geocode.LocatorGeocodeResult;
import com.esri.core.tasks.geocode.BatchGeocodeResult;
import com.esri.map.ArcGISTiledMapServiceLayer;
import com.esri.map.GraphicsLayer;
import com.esri.map.JMap;
import com.esri.map.LayerInitializeCompleteEvent;
import com.esri.map.LayerInitializeCompleteListener;
import com.esri.map.LayerList;

/***
 * This application shows how to perform geocoding - finding a location for
 * a given address - using the single line geocoding functionality presented
 * at version 10.1.1. A {@link LocatorFindParameters} object is created with
 * the single line to geocode (a String), then passed to the {@link Locator}
 * created for execution of the find task, which will return the locations 
 * found (synchronously or asynchronously, depending on the overload of the 
 * <code>find</code> method chosen).
 * <p>
 * To use the sample, choose a single line address from the combo box and
 * click the 'Find' button. The top 3 results will be shown, and the top
 * result will be shown on the map via a graphic. Click on the graphic
 * for an infopopup displaying the found location's attributes.
 */
public class AddressToLocation {

  private JComponent contentPane;
  private JMap map;
  private GraphicsLayer addressGraphics;
  private JComboBox<String> addressCombo;
  private ResultPanel resultPanel;
  private Locator locator;

  // symbology to show clicked locations
  private PictureMarkerSymbol symPoint;

  // UI settings
  private static final int NUM_RESULTS_TO_SHOW = 3;
  private final int COMPONENT_WIDTH = 320;
  private final int COMPONENT_HEIGHT = 25;
  private final String CLIENT_SECRET = "13f289f342cd422ebd51395d27d05230";
  private final String CLIENT_ID = "9JpposIsCzwIqMeE";

  // ------------------------------------------------------------------------
  // Constructor
  // ------------------------------------------------------------------------
  public AddressToLocation() {
    symPoint = new PictureMarkerSymbol(
        "http://static.arcgis.com/images/Symbols/Basic/RedShinyPin.png");
    symPoint.setSize(40, 40);
    symPoint.setOffsetY(20.0f);
  }

  // ------------------------------------------------------------------------
  // Core functionality
  // ------------------------------------------------------------------------
  private JButton getFindButton() {
    JButton findButton = new JButton("Find");
    // the action performed on button click
    findButton.addActionListener(new java.awt.event.ActionListener() {
      @Override
      public void actionPerformed(java.awt.event.ActionEvent event) {
        //set parameters including address selected in combo
        LocatorFindParameters params = new LocatorFindParameters(addressCombo.getSelectedItem().toString());
        params.setMaxLocations(NUM_RESULTS_TO_SHOW);
        params.setOutSR(map.getSpatialReference());

        List<Map<String,String>> addresses = new ArrayList<Map<String,String>>();
        for(int i = 0; i < 100; i++){
          addresses.add(new HashMap<String,String>());
          addresses.get(i).put("Address", "4181 Cray Dr.");
          addresses.get(i).put("City", "Warrenton");
          addresses.get(i).put("Region", "VA");
          addresses.get(i).put("Postal", "20187");
        }

        //run the locator find task asynchronously to not freeze the UI
        //locator.find(params, new CallbackListener<List<LocatorGeocodeResult>>() {
        Portal portal = new Portal("http://sgx.maps.arcgis.com/",null);
        try{
          portal.doOAuthAppAuthenticate(CLIENT_SECRET, CLIENT_ID, null);
          System.out.println("At it");
          UserCredentials uc = new UserCredentials();
          uc.setUserAccount("holster","bh108381");
          Locator loc = Locator.createOnlineLocator("http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer", uc);
          loc.batchGeocode(addresses, map.getSpatialReference());
          System.out.println("Past it");
        }catch(Exception e){
          e.printStackTrace();
        }
    }
  });
    return findButton;
  }

  // ------------------------------------------------------------------------
  // Public methods
  // ------------------------------------------------------------------------
  public JComponent createUI() {

    // application content
    contentPane = createContentPane();

    // map
    map = createMap();

    // user panel
    JPanel addressPanel = createAddressPanel();
    contentPane.add(addressPanel);

    // result panel
    resultPanel = new ResultPanel();
    resultPanel.setBounds(10, addressPanel.getHeight() + 20, COMPONENT_WIDTH, 120);
    contentPane.add(resultPanel);

    contentPane.add(map);

    return contentPane;
  }

  // ------------------------------------------------------------------------
  // Private methods
  // ------------------------------------------------------------------------
  private void displayResults(List<LocatorGeocodeResult> results) {

    if (results.size() == 0) {
      resultPanel.setTitle("Geocode result:");
      resultPanel.setContent("Sorry, there were no results for the location entered.");
      return;
    }
    // set the title for our query results panel
    resultPanel.setTitle("Top "+ results.size() + " Geocode result" + (results.size() == 1 ? "" : "s")+":");

    // build our result string
    StringBuilder text = new StringBuilder();

    for (int i = 0; i < results.size(); i++) {
      LocatorGeocodeResult result = results.get(i);
      String resultString = result.getAddress();
      text.append(resultString + "\n");
    }
    // set the result string as the content to our results panel
    resultPanel.setContent(text.toString());
    // make sure panel is visible - hidden at first
    resultPanel.setVisible(true);

    // get the top result to display on map
    LocatorGeocodeResult highestScoreResult = results.get(0);
    Point coords = highestScoreResult.getLocation();
    //Point coords = map.toMapPoint(fcoords.getX(),fcoords.getY());

    // create and populate attribute map
    Map<String, Object> attributes = new HashMap<>();
    attributes.put("Located address", highestScoreResult.getAddress());
    attributes.put("Locator score", Integer.valueOf(highestScoreResult.getScore()));
    attributes.put("Coordinates", String.valueOf(coords.getX())+", "+String.valueOf(coords.getY()));
    for (Entry<String, String> entry : highestScoreResult.getAttributes().entrySet())
    {
      attributes.put(entry.getKey(), entry.getValue());
    }

    // create a graphic at this location
    Graphic addressGraphic = new Graphic(
        highestScoreResult.getLocation(), symPoint, attributes);

    addressGraphics.addGraphic(addressGraphic);

    // centre the map at this location
    Envelope extent = map.getExtent();
    extent.centerAt(highestScoreResult.getLocation());
    map.zoomTo(extent);
  }

  private JMap createMap() {

    final JMap jMap = new JMap();

    //add data to the map
    ArcGISTiledMapServiceLayer worldLayer = new ArcGISTiledMapServiceLayer(
        "http://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer");
    LayerList layers = jMap.getLayers();
    layers.add(worldLayer);

    // zoom in to street level
    //jMap.setExtent(new Envelope(-13641663, 4540667, -13622897, 4553183));

    // create an InfoPopupOverlay to show attributes of the locations found
    final InfoPopupOverlay infoPopupOverlay =
        new InfoPopupOverlay();
    infoPopupOverlay.setPopupTitle("Address");
    infoPopupOverlay.setItemTitle("{Match_Addr}");
    infoPopupOverlay.setInitialPopupSize(new Dimension(300, 200));
    jMap.addMapOverlay(infoPopupOverlay);

    // create a graphics layer to show the locations found
    addressGraphics = new GraphicsLayer();
    addressGraphics.addLayerInitializeCompleteListener(new LayerInitializeCompleteListener() {
      @Override
      public void layerInitializeComplete(LayerInitializeCompleteEvent event) {
        infoPopupOverlay.addLayer(event.getLayer());
      }
    });

    layers.add(addressGraphics);

    // create Locator once
    locator = Locator.createOnlineLocator(
        "http://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer");

    return jMap;
  }

  /**
   * Creates a window; disposes of the map and locator when the window closes.
   * @return a window.
   */
  private JFrame createWindow() {
    JFrame window = new JFrame("Address to Location Application");
    window.setBounds(100, 100, 1000, 700);
    window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    window.getContentPane().setLayout(new BorderLayout(0, 0));
    window.addWindowListener(new WindowAdapter() {
      @Override
      public void windowClosing(WindowEvent windowEvent) {
        super.windowClosing(windowEvent);
        if (map != null) map.dispose();
        if (locator != null) locator.dispose();
      }
    });
    return window;
  }

  /**
   * Creates a content pane.
   * @return a content pane.
   */
  private static JLayeredPane createContentPane() {
    JLayeredPane contentPane = new JLayeredPane();
    contentPane.setBounds(100, 100, 1000, 700);
    contentPane.setLayout(new BorderLayout(0, 0));
    contentPane.setVisible(true);
    return contentPane;
  }

  private JPanel createAddressPanel() {
    JPanel panel = new JPanel();
    panel.setBounds(10, 10, COMPONENT_WIDTH, 100);
    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
    panel.setBackground(new Color(0, 0, 0, 120));
    panel.setBorder(BorderFactory.createEmptyBorder(5,10,5,10));
    JLabel topText = new JLabel("Select or enter an address, then click 'Find':");
    topText.setForeground(Color.WHITE);
    topText.setAlignmentX(Component.LEFT_ALIGNMENT);
    topText.setPreferredSize(new Dimension(COMPONENT_WIDTH, COMPONENT_HEIGHT));

    // Create a combo box with some SF addresses
    String[] addresses = new String[]
        {
        "1455 MARKET ST, SAN FRANCISCO, CA 94103",
        "2011 MISSION ST, SAN FRANCISCO, CA 94110",
        "820 BRYANT ST, SAN FRANCISCO, CA 94103",
        };
    addressCombo = new JComboBox<>();
    addressCombo.setModel(new DefaultComboBoxModel<>(addresses));
    addressCombo.setAlignmentX(Component.LEFT_ALIGNMENT);
    addressCombo.setPreferredSize(new Dimension(COMPONENT_WIDTH, COMPONENT_HEIGHT));
    addressCombo.setEditable(true); // so users can type in an address of their choice

    // button
    JButton findButton = getFindButton();
    findButton.setPreferredSize(new Dimension(COMPONENT_WIDTH, COMPONENT_HEIGHT));

    panel.add(topText);
    panel.add(addressCombo);
    panel.add(Box.createRigidArea(new Dimension(0,5)));
    panel.add(findButton);
    panel.setVisible(true);
    return panel;
  }
  
  private String wrap(String str) {
    // create a HTML string that wraps text when longer
    return "<html><p style='width:200px;'>" + str + "</html>";
  }

  // ------------------------------------------------------------------------
  // Static methods
  // ------------------------------------------------------------------------
  /**
   * Starting point of this application.
   * @param args arguments to this application.
   */
  public static void main(String[] args) {
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        try {
          // instance of this application
          AddressToLocation addressToLocationApp = new AddressToLocation();

          // create the UI, including the map, for the application.
          JFrame appWindow = addressToLocationApp.createWindow();
          appWindow.add(addressToLocationApp.createUI());
          appWindow.setVisible(true);
        } catch (Exception e) {
          // on any error, display the stack trace.
          e.printStackTrace();
        }
      }
    });
  }
}