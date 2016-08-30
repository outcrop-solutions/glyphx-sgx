using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using CsvHelper;

namespace IngestionEngine
{
    public partial class Form_SmartDecimation : Form
    {
        bool GPS_Selected = false;
        bool LAS_Selected = false;
        string gpsFile = "";
        string lasFile = "";
        string outFile = "";

        public Form_SmartDecimation()
        {
            InitializeComponent();
        }

        private void btx_Run_Click(object sender, EventArgs e)
        {
            CsvReader csvResult;
            // CsvReader cvsLAS;
            List<gps_las_Data> glData = new List<gps_las_Data>();
            gps_las_Data glDatum = new gps_las_Data();
            gps_las_Data lasPt = new gps_las_Data();
            StreamWriter sw; //, sw2;
            bool gotGPS_Data = false;
            // decimal tmp_elv, tmp_lat, tmp_lon;
            int id = 0;


            if (GPS_Selected && LAS_Selected)
            {   
                //saveFileDialog1.FileName;
                
                bool keep_this_data_point = false;

                MessageBox.Show("Distance = " + diDistance.Value.ToString());
                //saveFileDialog1.ShowDialog();
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    tbOUTFile.Visible = true;
                    tbOUTFile.Text = "Saving results as: " + saveFileDialog1.FileName;
                    outFile = saveFileDialog1.FileName;
                    // open the gps file and read it into a list
                    // open the las file and process
                    // for each LAS line lat lon elv
                    //   for each gps list item
                    //       calculate-
                    //       if (las_elv < 250) AND 
                    //          ( gps_lat - las_lat < distance ) OR
                    //          ( gps_lon - las_log < distance ) OR
                    //          ( gps_elv - las_elv < distance ) 
                    //          save las pt = true;
                    
            
                    try {
                        using (csvResult = new CsvHelper.CsvReader(new StreamReader(gpsFile)))
                        {
                            csvResult.Read(); // this is the GPS header
                            // Ide,lat, lon, AltitudeMeters, DistanceMeters, HeartRateBpm, Cadence, Speed, cal,Cadence_derv
                            do{
                                glDatum.lat = Convert.ToDecimal(csvResult.GetField("lat"));
                                glDatum.lon = Convert.ToDecimal(csvResult.GetField(" lon"));
                                glDatum.elv = Convert.ToDecimal(csvResult.GetField(" AltitudeMeters"));
                                glData.Add(glDatum);
                            } while (csvResult.Read());
                            gotGPS_Data = true;
                        }
                    }
                    catch{
                        gotGPS_Data = false;
                        MessageBox.Show("Could Not Read GPS Data File");
                    }

                    if (gotGPS_Data)
                    {
                        using (CsvReader csvLAS = new CsvReader(new StreamReader(lasFile))) 
                        using (sw = new StreamWriter(outFile))
                        //using (sw2 = new StreamWriter(outFile + ".smaller"))
                        
                        {
                            csvLAS.Read();
                            // header
                            // lat ,  lon ,  elev ,  r ,  g ,  b ,  rownum
                            sw.WriteLine("lat, lon, elev, r, g, b, rownum");
                            //sw2.WriteLine("lat, lon, elev, r, g, b, rownum");
                            do
                            {
                                lasPt.lat = Convert.ToDecimal(csvLAS.GetField("lat "));
                                lasPt.lon = Convert.ToDecimal(csvLAS.GetField(" lon "));
                                lasPt.elv = Convert.ToDecimal(csvLAS.GetField(" elev "));
                                lasPt.r = Convert.ToInt16(csvLAS.GetField(" r "));
                                lasPt.g = Convert.ToInt16(csvLAS.GetField(" g "));
                                lasPt.b = Convert.ToInt16(csvLAS.GetField(" b "));
                                lasPt.id = Convert.ToDecimal(csvLAS.GetField(" rownum"));
                                if ((id % diSkipFactor.Value) == 0)
                                {

                                    foreach (gps_las_Data gpt in glData)
                                    {
                                        if ( ( lasPt.elv < Convert.ToDecimal(diElevation.Value) ) ) {
                                            if (( Math.Abs( lasPt.lat - gpt.lat) < Convert.ToDecimal(diDistance.Value)) ||
                                                ( Math.Abs( lasPt.lon - gpt.lon) < Convert.ToDecimal(diDistance.Value)) ||
                                                ( Math.Abs( lasPt.elv - gpt.elv) < Convert.ToDecimal(diElevation.Value))
                                               )
                                            {
                                                keep_this_data_point = true ;
                                            }
                                        }
                                    }
                                    if (keep_this_data_point)
                                        sw.WriteLine(lasPt.lat + ", " + lasPt.lon + ", " + lasPt.elv + ", " + lasPt.r + ", " + lasPt.g + ", " + lasPt.b + ", " + lasPt.lat + ", " + lasPt.lat);
                                    keep_this_data_point = false;
                                }
                                id++;
                            } while (csvLAS.Read());
                            //sw.Close();
                        }

                    } // if got gps data
                    
                }
                else
                {
                    MessageBox.Show("Action Canceled");
                }
                
                
            }
        }

        private void btXGPS_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                gpsFile = openFileDialog1.FileName;
                tbGPSFile.Text = gpsFile;
                GPS_Selected = true;
                if (GPS_Selected && LAS_Selected) btX_Run.Enabled = true;
            }
        }

        private void btXLAS_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                lasFile = openFileDialog1.FileName;
                tbLASFile.Text = lasFile;
                LAS_Selected = true;
                if (GPS_Selected && LAS_Selected) btX_Run.Enabled = true;
            }
        }
    }

    class gps_las_Data
    {
        public decimal lat { get; set; }
        public decimal lon { get; set; }
        public decimal elv { get; set; }
        public int r { get; set; }
        public int g { get; set; }
        public int b { get; set; }
        public decimal id { get; set; }
        public string other { get; set; }
    }
}
