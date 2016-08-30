using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IngestionEngine
{
    static class Program
    {
        public static Dictionary<string, double> AutoScalerDefaults = new Dictionary<string, double>();
        public static Dictionary<String, Palette_Glyph_Class> PaletteList = new Dictionary<string, Palette_Glyph_Class>();
        public static Dictionary<string, string> DP_list_items = new Dictionary<string, string>();  // Dropdown Palette List Items
        public static Dictionary<string, string> DC_list_items = new Dictionary<string, string>();  // Dropdown Palette List Items
        public static Dictionary<string, colStats> colData = new Dictionary<string, colStats>();
        public static bool gDiff1 = false;
        public static bool gDiff2 = false;
        public static bool DiffGlyphMappingWorkflow = false;
        public static bool OK_to_Diff_Map = false;
        public static bool wfMainStep1 = false;
        public static bool wfMainStep2 = false;
        public static bool wfMainStep3 = false;
        public static bool wfMainStep4 = false;
        public static bool gDiffUse1file = true;
        public static int NumberOfLevels = 0;


        /// <summary> 
        /// The main entry point for the application.
        /// </summary>
        [STAThread]

        static void Main()
        {
            // glyph defaults
            AutoScalerDefaults.Add("translate_x_max", 180.0);
            AutoScalerDefaults.Add("translate_x_min", -180.0);
            AutoScalerDefaults.Add("translate_y_max", 90.0);
            AutoScalerDefaults.Add("translate_y_min", -90.0);
            AutoScalerDefaults.Add("translate_z_max", 10000.0);
            AutoScalerDefaults.Add("translate_z_min", 0.0);
            AutoScalerDefaults.Add("color_r_max", 255.0);
            AutoScalerDefaults.Add("color_r_min", 0.0);
            AutoScalerDefaults.Add("color_g_max", 255.0);
            AutoScalerDefaults.Add("color_g_min", 0.0);
            AutoScalerDefaults.Add("color_b_max", 255.0);
            AutoScalerDefaults.Add("color_b_min", 0.0);
            AutoScalerDefaults.Add("color_a_max", 255.0);
            AutoScalerDefaults.Add("color_a_min", 0.0);
            AutoScalerDefaults.Add("color_index_max", 255.0);
            AutoScalerDefaults.Add("color_index_min", 0.0);
            AutoScalerDefaults.Add("rotate_max", 360.0);
            AutoScalerDefaults.Add("rotate_min", 0.0);
            AutoScalerDefaults.Add("spin_max", 1.0);
            AutoScalerDefaults.Add("spin_min", -1.0);
            AutoScalerDefaults.Add("orbit_max", 1.0);
            AutoScalerDefaults.Add("orbit_min", -1.0);
            AutoScalerDefaults.Add("geometry_max", 19.0);
            AutoScalerDefaults.Add("geometry_min", 0.0);
            AutoScalerDefaults.Add("topology_max", 7.0);
            AutoScalerDefaults.Add("topology_min", 0.0);

            // palatteList default
            Palette_Glyph_Class default1 = new Palette_Glyph_Class();
            Palette_Glyph_Class default2 = new Palette_Glyph_Class();
            Palette_Glyph_Class default3 = new Palette_Glyph_Class();
            Palette_Glyph_Class default4 = new Palette_Glyph_Class();
            Palette_Glyph_Class default5 = new Palette_Glyph_Class();
            Palette_Glyph_Class default6 = new Palette_Glyph_Class();
            
            default1.lowColor = System.Drawing.Color.Black;
            default1.lowNumber = 0;
            default1.hiColor = System.Drawing.Color.White;
            default1.hiNumber = 255;
            default1.name = "White to Black";
            PaletteList.Add(default1.name, default1);

            default2.lowColor = System.Drawing.Color.LavenderBlush;
            default2.lowNumber = 0;
            default2.hiColor = System.Drawing.Color.Red;
            default2.hiNumber = 255;
            default2.name = "LavenderBlush to Red";
            PaletteList.Add(default2.name, default2);

            default3.lowColor = System.Drawing.Color.ForestGreen;
            default3.lowNumber = 0;
            default3.hiColor = System.Drawing.Color.LightSeaGreen;
            default3.hiNumber = 255;
            default3.name = "ForestGreen to LightSeaGreen";
            PaletteList.Add(default3.name, default3);

            default4.lowColor = System.Drawing.Color.DarkOrange;
            default4.lowNumber = 0;
            default4.hiColor = System.Drawing.Color.LightYellow;
            default4.hiNumber = 255;
            default4.name = "Dark Orange to Light Yellow";
            PaletteList.Add(default4.name, default4);

            default5.lowColor = System.Drawing.Color.Navy;
            default5.lowNumber = 0;
            default5.hiColor = System.Drawing.Color.Aqua;
            default5.hiNumber = 255;
            default5.name = "Blues Brothers";
            PaletteList.Add(default5.name, default5);

            default6.lowColor = System.Drawing.Color.DeepPink;
            default6.lowNumber = 0;
            default6.hiColor = System.Drawing.Color.LightPink;
            default6.hiNumber = 255;
            default6.name = "Fifty Shades of Pink";
            PaletteList.Add(default6.name, default6);




            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
        
    }
}
