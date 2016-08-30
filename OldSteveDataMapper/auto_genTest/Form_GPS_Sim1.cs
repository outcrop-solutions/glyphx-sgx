using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;
using IngestionEngine.net.usgs.gisdata;

namespace IngestionEngine
{
    public partial class Form_GPS_Sim1 : Form
    {
        //List<string> _waypoints = new List<string>(); // <-- Add this
        
        List<wayPoint> wayPts = new List<wayPoint>();
        int indx = 0;
        
        public class wayPoint {
            public string name { get; set; }
            public double lat { get; set; }
            public double lon { get; set; }
            public double ele { get; set; }
            public double stp { get; set; }
            public double hrt { get; set; }
            public double cad { get; set; }
            public double vo2 { get; set; }
            public double cal { get; set; }
            public override string ToString()
            {
                return string.Format("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}", name, lat, lon, ele, stp, hrt, cad, vo2, cal);
            }
            public string thisPlay { get {return this.ToString();} }
        }
        public Form_GPS_Sim1()
        {
            InitializeComponent();
            //_waypoints.Add("New York, New Yourque");

            lbWayPts.DataSource = wayPts;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            wayPoint way = new wayPoint();
            way.name = indx.ToString();
            way.lat = diLat.Value;
            way.lon = diLon.Value;
            way.ele = diElv.Value;
            way.stp = diStp.Value;
            way.hrt = diHrt.Value;
            way.cad = diCad.Value;
            way.vo2 = diVo2.Value;
            way.cal = diCal.Value;
            wayPts.Add(way);
            indx++;
            lbWayPts.DataSource = null;
            lbWayPts.DataSource = wayPts;
            lbWayPts.DisplayMember = "thisPlay";

        }

        private void buttonX1_Click(object sender, EventArgs e)
        {
            int i = 0;
            double delta = 0.0;
            string outstr = "Results:\r\n";
            wayPoint[] wary = wayPts.ToArray();

            if (wayPts.Count > 1)
            {
                outstr += "Latitude, Longitude, Elevation, HeartRate, Cadance, Vo2, CaloriesEarned\r\n";
                //outstr += string.Format("{0}, {1}, {2}, {3}, {4}, {5}, {6} \r\n",
                //            wary[0].lat,
                //            wary[0].lon,
                //            wary[0].ele,
                //            wary[0].hrt,
                //            wary[0].cad,
                //            wary[0].vo2,
                //            wary[0].cal);
                for ( i = 0; i < wary.Length - 1; i++) 
                //foreach( wayPoint wp in wayPts)
                {  //output first waypoint

                    delta = 1 / wary[i].stp;
                    for (int j = 0; j < (int)wary[i].stp; j++)
                    {
                        outstr += string.Format("{0}, {1}, {2}, {3}, {4}, {5}, {6} \r\n", 
                            interpolate(wary[i].lat, wary[i + 1].lat, (j * delta)),
                            interpolate(wary[i].lon, wary[i + 1].lon, (j * delta)),
                            interpolate(wary[i].ele, wary[i + 1].ele, (j * delta)), 
                            interpolate(wary[i].hrt, wary[i + 1].hrt, (j * delta)),
                            interpolate(wary[i].cad, wary[i + 1].cad, (j * delta)),
                            interpolate(wary[i].vo2, wary[i + 1].vo2, (j * delta)), 
                            interpolate(wary[i].cal, wary[i + 1].cal, (j * delta)));
                    }
                }
                // output last waypoint
                outstr += string.Format("{0}, {1}, {2}, {3}, {4}, {5}, {6} \r\n",
                            wary[wary.Length - 1].lat,
                            wary[wary.Length - 1].lon,
                            wary[wary.Length - 1].ele,
                            wary[wary.Length - 1].hrt,
                            wary[wary.Length - 1].cad,
                            wary[wary.Length - 1].vo2,
                            wary[wary.Length - 1].cal);
            }
            else
            {
                outstr += "Add more Waypoints Please.\r\n";
            }
            tbResults.Text = outstr;
        }

        Int32 interpolate(Int32 a, Int32 b, double p)
        {
            return (Int32)(a * (1 - p) + b * p);
        }
        double interpolate(double a, double b, double p)
        {
            return (double)(a * (1 - p) + b * p);
        }


        public class pointtest
        {
       //     string route = "lines": [
       //{
       //   "distance": 0.22555152907022982,
       //   "bearing": 0,
       //   "points": [
       //              { "lon": -96.79942, "lat": 32.81145 },
       //              { "lon": -96.80004, "lat": 32.81079 },
       //              { "lon": -96.80042, "lat": 32.8103 },
       //              { "lon": -96.80071, "lat": 32.80979 },
       //              { "lon": -96.80078, "lat": 32.80976 },
       //              { "lon": -96.80098, "lat": 32.80934 },
       //              { "lon": -96.80095, "lat": 32.80927 },
       //              { "lon": -96.8011 , "lat": 32.80898 },
       //              { "lon": -96.80125, "lat": 32.80864 },
       //              { "lon": -96.80209, "lat": 32.80691 },
       //              { "lon": -96.80215, "lat": 32.80689 },
       //              { "lon": -96.80232, "lat": 32.80649 },
       //              { "lon": -96.80231, "lat": 32.80642 }
    //                ]
    //} ];
        }
        public class pointers
        {
            
            [JsonProperty("lon")]
            public Double lon { get; set; }
            [JsonProperty("lat")]
            public Double lat { get; set; }
        }
        public class lines
        {
            [JsonProperty("distance")]
            public Double distance { get; set; }
            [JsonProperty("bearing")]
            public Double bearing { get; set; }
            [JsonProperty("points")]
            public List<pointers> points { get; set; }

        }
        private void btReadRoute_Click(object sender, EventArgs e)
        {
            List<lines> linelist = new List<lines>();
            lines liner = new lines();
            String ostr = "";
            indx = 0;
            double x = -122.89478301998749;
            double y = 47.00290950656123;
            string unit = "FEET"; // "METERS"
            // string sourceLayer = "*Elev_WA";
            string sourceLayer = "-1";
            System.Xml.XmlNode results;

            using (net.usgs.gisdata.Elevation_Service elevasvc = new Elevation_Service())
            {
                results = elevasvc.getElevation(x.ToString(), y.ToString(), unit, sourceLayer, "TRUE");
            }

            double elevation = double.Parse(results.FirstChild.Value);
 
            Console.WriteLine(elevation);	
            
            using (StreamReader r = new StreamReader("c:\\Code_hause\\__SynglyphX\\data_Raytheon\\run_files\\KTPerryTrailJSON.txt"))
            {
                string json = r.ReadToEnd();
                linelist = JsonConvert.DeserializeObject<List<lines> >(json);
            }
            tbResults.Text = linelist.ToString();
            ostr += "**********************\r\nCalling elevation =" + elevation + "\r\n";
            foreach (lines lin in linelist)
            {
                ostr += "Bearing = " + lin.bearing + "\r\n";
                ostr += "Distance = " + lin.distance + "\r\n";
                foreach (pointers poynt in lin.points)
                {
                    using (net.usgs.gisdata.Elevation_Service elevasvc = new Elevation_Service())
                    {
                        results = elevasvc.getElevation(poynt.lon.ToString(), poynt.lat.ToString(), unit, sourceLayer, "TRUE");
                    }
                    wayPoint way = new wayPoint();
                    elevation = double.Parse(results.FirstChild.Value);
                    ostr += "\tPoints = Lon: " + poynt.lon + " Lat: " + poynt.lat + " elevation: " + elevation + "\r\n";
                    way.lat = poynt.lat;
                    way.lon = poynt.lon;
                    way.ele = elevation;
                    way.name = indx.ToString();
                    way.stp = diStp.Value;
                    way.hrt = diHrt.Value;
                    way.cad = diCad.Value + (indx % 10) + indx;
                    way.vo2 = indx;
                    way.cal = diCal.Value + indx;
                    wayPts.Add(way);
                    indx++;
                }
            }
            tbResults.Text = ostr;
            lbWayPts.DataSource = null;
            lbWayPts.DataSource = wayPts;
            lbWayPts.DisplayMember = "thisPlay";
        }
        
        
    }
}
