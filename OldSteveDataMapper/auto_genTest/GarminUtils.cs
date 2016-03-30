using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace IngestionEngine
{
    public class GarminUtils
    {
        public static Activity ConvertTCS(string fileName)
        {
            XElement root = XElement.Load(fileName);
            XNamespace ns1 = "http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2";
            XNamespace ns2 = "http://www.garmin.com/xmlschemas/ActivityExtension/v2";
                //http://www.garmin.com/xmlschemas/ActivityExtension/v2
            // http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2

            IEnumerable<Activity> activities = from activityElement in root.Descendants(ns1 + "Activities")
                                               select new Activity
                                                       {
                                                           Laps =
                                                               (from lapElement in activityElement.Descendants(ns1 + "Lap")
                                                                select new Lap
                                                                            {
                                                                                TotalTimeSeconds =
                                                                                    lapElement.Element(ns1 + "TotalTimeSeconds") != null ? Convert.ToDouble((string)lapElement.Element(ns1 + "TotalTimeSeconds").Value) : 0.00,
                                                                                DistanceMeters =
                                                                                    lapElement.Element(ns1 + "DistanceMeters") != null ? Convert.ToDouble((string)lapElement.Element(ns1 + "DistanceMeters").Value) : 0.00,
                                                                                MaximumSpeed =
                                                                                    lapElement.Element(ns1 + "MaximumSpeed") != null ? Convert.ToDouble((string)lapElement.Element(ns1 + "MaximumSpeed").Value) : 0.00,
                                                                                Calories =
                                                                                    lapElement.Element(ns1 + "Calories") != null ? Convert.ToInt16((string)lapElement.Element(ns1 + "Calories").Value) : 0,
                                                                                AverageHeartRateBpm =
                                                                                    lapElement.Element(ns1 + "AverageHeartRateBpm") != null ? Convert.ToInt16((string)lapElement.Element(ns1 + "AverageHeartRateBpm").Value) : 0,
                                                                                MaximumHeartRateBpm =
                                                                                    lapElement.Element(ns1 + "MaximumHeartRateBpm") != null ? Convert.ToInt16((string)lapElement.Element(ns1 + "MaximumHeartRateBpm").Value) : 0,
                                                                                Intensity =
                                                                                    lapElement.Element(ns1 + "Intensity") != null ? lapElement.Element(ns1 + "Intensity").Value : "",
                                                                                //Cadence =
                                                                                //    lapElement.Element(ns1 + "RunCadence") != null ? Convert.ToInt16((string)lapElement.Element(ns1 + "RunCadence").Value) : 0,
                                                                                TriggerMethod =
                                                                                    lapElement.Element(ns1 + "TriggerMethod") != null ? lapElement.Element(ns1 + "TriggerMethod").Value : "",
                                                                                Notes =
                                                                                    lapElement.Element(ns1 + "Notes") != null ? lapElement.Element(ns1 + "Notes").Value : "",
                                                                                Tracks =
                                                                                    (from trackElement in lapElement.Descendants(ns1 + "Track")
                                                                                     select new Track
                                                                                        {
                                                                                            TrackPoints =
                                                                                                (from trackPointElement in trackElement.Descendants(ns1 + "Trackpoint")
                                                                                                 select new TrackPoint
                                                                                                         {
                                                                                                             Timex = trackPointElement.Element(ns1 + "Time") != null ? Convert.ToString((string)trackPointElement.Element(ns1 + "Time").Value) : "",
                                                                                                             AltitudeMeters =
                                                                                                                 trackPointElement.Element(ns1 + "AltitudeMeters") != null ? Convert.ToDecimal((string)trackPointElement.Element(ns1 + "AltitudeMeters").Value) : (decimal)0.0,
                                                                                                             DistanceMeters =
                                                                                                                 trackPointElement.Element(ns1 + "DistanceMeters") != null ? Convert.ToDecimal((string)trackPointElement.Element(ns1 + "DistanceMeters").Value) : 0.0M,
                                                                                                             HeartRateBpm =
                                                                                                                 trackPointElement.Element(ns1 + "HeartRateBpm") != null ? Convert.ToInt16((string)trackPointElement.Element(ns1 + "HeartRateBpm").Value) : 0,
                                                                                                             Cadence =
                                                                                                                 trackPointElement.Element(ns1 + "Cadence") != null ?
                                                                                                                 Convert.ToInt16((string)trackPointElement.Element(ns1 + "Cadence").Value)
                                                                                                                 :
                                                                                                                 //trackPointElement.Element(ns1 + "Extensions").Element(ns2 + "TPX").Element(ns2 + "RunCadence") != null ? Convert.ToInt16((string)trackPointElement.Element(ns1 + "Extensions").Element(ns2 + "TPX").Element(ns2 + "RunCadence").Value) 
                                                                                                                 //: 
                                                                                                                 
                                                                                                                 0,
                                                                                                             Speed =
                                                                                                                ((trackPointElement.Element(ns1 + "Extensions")!= null ) &&
                                                                                                                 (trackPointElement.Element(ns1 + "Extensions").Element(ns2 + "TPX").Element(ns2 + "Speed") != null ))? Convert.ToDecimal((string)trackPointElement.Element(ns1 + "Extensions").Element(ns2 + "TPX").Element(ns2 + "Speed").Value) : 0M,

                                                                                                             SensorState =
                                                                                                                 trackPointElement.Element(ns1 + "SensorState") != null ? trackPointElement.Element(ns1 + "SensorState").Value : "",

                                                                                                             //Positionx = ((trackPointElement.Element(ns1 + "Position") != null))
                                                                                                             //              ?
                                                                                                             //                ((
                                                                                                             //                    from positionElement in
                                                                                                             //                        trackPointElement.Descendants(ns1 + "Position")
                                                                                                             //                    select
                                                                                                             //                        new Position
                                                                                                             //                            {
                                                                                                             //                                LatitudeDegrees = Convert.ToDecimal((string)
                                                                                                             //                                    positionElement.Element(ns1 + "LatitudeDegrees") ?? ""),
                                                                                                             //                                LongitudeDegrees = Convert.ToDecimal((string)
                                                                                                             //                                    positionElement.Element(ns1 + "LongitudeDegrees") ?? "")
                                                                                                             //                            }).ToList()
                                                                                                             //                )
                                                                                                             //              : new List<Position>{ (new Position { LatitudeDegrees = 0M, LongitudeDegrees = 0M})}

                                                                                                             Positionx = ((
                                                                                                                 from positionElement in
                                                                                                                     trackPointElement.Descendants(ns1 + "Position")
                                                                                                                 select
                                                                                                                     new Position
                                                                                                                         {
                                                                                                                             LatitudeDegrees = Convert.ToDecimal((string)
                                                                                                                                 positionElement.Element(ns1 + "LatitudeDegrees") ?? ""),
                                                                                                                             LongitudeDegrees = Convert.ToDecimal((string)
                                                                                                                                 positionElement.Element(ns1 + "LongitudeDegrees") ?? "")
                                                                                                                         }).ToList())
                                                                                                         }).ToList()
                                                                                        }).ToList()
                                                                            }).ToList()
                                                       };

            return activities.SingleOrDefault();
        }
    }
    public class Activity
    {
        public string Id { set; get; }
        public string Sport { set; get; }
        public List<Lap> Laps { set; get; }
    }

    public class Lap
    {
        public double TotalTimeSeconds { set; get; }
        public double DistanceMeters { set; get; }
        public double MaximumSpeed { set; get; }
        public int Calories { set; get; }
        public string TriggerMethod { set; get; }
        public int AverageHeartRateBpm { set; get; }
        public int MaximumHeartRateBpm { set; get; }
        public string Intensity { set; get; }
        public int Cadence { set; get; }
        public string Notes { set; get; }

        public List<Track> Tracks { set; get; }
    }
    public class Track
    {
        public List<TrackPoint> TrackPoints { set; get; }
    }
    public class TrackPoint
    {
        public string Timex { set; get; }
        public decimal AltitudeMeters { get; set; }
        public decimal DistanceMeters { get; set; }
        public int HeartRateBpm { get; set; }
        public int Cadence { get; set; }
        public decimal Speed { get; set; }
        public string SensorState { get; set; }
        public List<Position> Positionx { get; set; }
        public List<Extens> Extras { get; set; }
    }
    public class Position
    {
        public Decimal LatitudeDegrees { get; set; }
        public Decimal LongitudeDegrees { get; set; }
    }
    public class Extens
    {
        public Decimal Cadance { get; set; }
        public Decimal Speedy { get; set; } 
    }
    public class tcxPoint
    {
        public string name { get; set; }
        public decimal lat { get; set; }
        public decimal lon { get; set; }
        public decimal AltitudeMeters { get; set; }
        public string Timex { set; get; }
        public decimal DistanceMeters { get; set; }
        public int HeartRateBpm { get; set; }
        public decimal Cadence { get; set; }
        public decimal Speed { get; set; }
        public double cal { get; set; }
        public override string ToString()
        {
            return string.Format("{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}",
                name, lat, lon, AltitudeMeters, Timex, DistanceMeters, HeartRateBpm, Cadence, Speed, cal);
        }
        public string thisPlay { get { return this.ToString(); } }
    }
}
