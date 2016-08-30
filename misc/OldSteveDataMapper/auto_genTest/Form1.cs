using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using CsvHelper;
using CsvHelper.Configuration;
using System.IO;
using System.Threading;
using System.Globalization;
using System.Diagnostics;
namespace IngestionEngine
{
    public partial class Form1 : Form
    {
        public int goarounds = 0;
        public List<Control> glyphLevelControls = new List<Control>();
        public List<TabPage> myTabList = new List<TabPage>();
        public Dictionary<int, TabPage> myTabDict = new Dictionary<int, TabPage>();
        public Dictionary<string, string> DD_list_items = new Dictionary<string, string>();  // Drop Down List Items
        public Dictionary<string, string> DG_list_items = new Dictionary<string, string>();  // Dropdown Geometry List Items
        public Dictionary<string, string> DT_list_items = new Dictionary<string, string>();  // Dropdown Topography List Items
        public Dictionary<string, string> DC_list_items = new Dictionary<string, string>();  // Dropdown Throb Channel List Items
        public String inFile;
        public String outFile;
        public Boolean csv_FileOpened = false;
        public Boolean numberOfLevelsSet = false;
        public int numLevels = 0; 
        public Boolean numberOfGlyphsPerLevelsSet = false;
        public Ampz[,] ampzarray = new Ampz[17,17];
        public Dictionary<string, string> dropBoxItems = new Dictionary<string, string>();  // Possible Candidate for Removal
        public DataSet dSet = new DataSet();
        public CsvReader csvResult;
        public CsvReader csvResult1;
        public CsvReader csvResult2;
        public int[] glyphsPerLevel = new int[32];
        public int counter = 0;
        public int rownum = 0;
        public StreamWriter sw, swchan ;
        

        public Form1()
        {
            Thread t = new Thread(new ThreadStart(SplashStart));
            t.Start();
            Thread.Sleep(4000);
            InitializeComponent();
            t.Abort();
            timer1.Stop();
            timer1.Dispose();
            
            myTabList.Add(tabStart);        myTabDict.Add(-1, tabStart);
            myTabList.Add(tabCSVData);      myTabDict.Add(-2, tabCSVData);
            myTabList.Add(tab_L0);          myTabDict.Add( 0, tab_L0);
            myTabList.Add(tab_L1);          myTabDict.Add( 1, tab_L1);
            myTabList.Add(tab_L2);          myTabDict.Add( 2, tab_L2);
            myTabList.Add(tab_L3);          myTabDict.Add( 3, tab_L3);
            myTabList.Add(tab_L4);          myTabDict.Add( 4, tab_L4);
            myTabList.Add(tab_L5);          myTabDict.Add( 5, tab_L5);
            myTabList.Add(tab_L6);          myTabDict.Add( 6, tab_L6);
            myTabList.Add(tab_L7);          myTabDict.Add( 7, tab_L7);
            myTabList.Add(tab_L8);          myTabDict.Add( 8, tab_L8);
            myTabList.Add(tab_L9);          myTabDict.Add( 9, tab_L9);
            myTabList.Add(tab_L10);         myTabDict.Add(10, tab_L10);
            myTabList.Add(tab_L11);         myTabDict.Add(11, tab_L11);
            myTabList.Add(tab_L12);         myTabDict.Add(12, tab_L12);
            myTabList.Add(tab_L13);         myTabDict.Add(13, tab_L13);
            myTabList.Add(tab_L14);         myTabDict.Add(14, tab_L14);
            myTabList.Add(tab_L15);         myTabDict.Add(15, tab_L15);
            myTabList.Add(tab_GlyphDiff);

            exTC1.HideTabPage(tab_L0);      exTC1.HideTabPage(tab_L1);
            exTC1.HideTabPage(tab_L2);      exTC1.HideTabPage(tab_L3);
            exTC1.HideTabPage(tab_L4);      exTC1.HideTabPage(tab_L5);
            exTC1.HideTabPage(tab_L6);      exTC1.HideTabPage(tab_L7);
            exTC1.HideTabPage(tab_L8);      exTC1.HideTabPage(tab_L9);
            exTC1.HideTabPage(tab_L10);     exTC1.HideTabPage(tab_L11);
            exTC1.HideTabPage(tab_L12);     exTC1.HideTabPage(tab_L13);
            exTC1.HideTabPage(tab_L14);     exTC1.HideTabPage(tab_L15);
            exTC1.HideTabPage(tab_GlyphDiff);

            // drop down item list temp till data read
            //x,y,z,size1,color1,Rot1,Shape1,size2,color2,Rot2,Shape2
            DD_list_items.Add("", "");
            // Drop down item list specifically loaded with antz default geoms
            DG_list_items.Add("", "");
            DG_list_items.Add("7", "Toroid");
            DG_list_items.Add("6", "Toroid Wire");
	        DG_list_items.Add("0", "Cube Wire");
	        DG_list_items.Add("1", "Cube");	
            DG_list_items.Add("2", "Sphere Wire");
	        DG_list_items.Add("3", "Sphere");	
	        DG_list_items.Add("4", "Cone Wire");
	        DG_list_items.Add("5", "Cone");		
	        DG_list_items.Add("8", "Dodecahedron Wire");
	        DG_list_items.Add("9", "Dodecahedron");		
	        DG_list_items.Add("10", "Octahedron Wire");
	        DG_list_items.Add("11", "Octahedron");		
	        DG_list_items.Add("12", "Tetrahedron Wire");
	        DG_list_items.Add("13", "Tetrahedron");		
	        DG_list_items.Add("14", "Icosahedron Wire");
	        DG_list_items.Add("15", "Icosahedron");	
	        DG_list_items.Add("16", "Pin");			
	        DG_list_items.Add("17", "PinWire");		
	        DG_list_items.Add("18", "Cylinder Wire");
	        DG_list_items.Add("19", "Cylinder");
            DG_list_items.Add("20", "Teapot");
            DG_list_items.Add("21", "Teapot Wire");
            
            // Drop down item list specifically loaded with antz default Topology
            DT_list_items.Add("", "");
            DT_list_items.Add("1", "Cube");
            DT_list_items.Add("2", "Sphere");
            DT_list_items.Add("3", "Torus");
            DT_list_items.Add("4", "Cylinder");
            DT_list_items.Add("5", "Pin");
            DT_list_items.Add("6", "Rod");
            DT_list_items.Add("7", "Point");

            // Drop down list for channel output items.
            DC_list_items.Add("",   "");
            DC_list_items.Add("1",  "1");
            DC_list_items.Add("2",  "2");
            DC_list_items.Add("3",  "3");
            DC_list_items.Add("4",  "4");
            DC_list_items.Add("5",  "5");
            DC_list_items.Add("6",  "6");
            DC_list_items.Add("7",  "7");
            DC_list_items.Add("8",  "8");
            DC_list_items.Add("9",  "9");
            DC_list_items.Add("10", "10");
            DC_list_items.Add("11", "11");
            DC_list_items.Add("12", "12");
            DC_list_items.Add("13", "13");
            DC_list_items.Add("14", "14");
            DC_list_items.Add("15", "15");

            for (int ii = 0; ii < 17; ii++)
            {
                for (int jj = 0; jj < 17; jj++) {
                    ampzarray[ii, jj] = new Ampz();
                }
            }
            for (int ii = 0; ii < 32; ii++)
                glyphsPerLevel[ii] = 0;

            Program.DP_list_items.Add("", "");
            foreach ( string Kpals in Program.PaletteList.Keys  )
            {
                Program.DP_list_items.Add(Kpals.ToString(), Kpals.ToString());
            }
        }

        public void SplashStart()
        {
            Application.Run(new SplashForm());
        }
        
        private void setNumLevels_Click(object sender, EventArgs e)
        {

            numLevels = trackBar1.Value;
            label1.Text = numLevels.ToString();
            labelNumLevels.Text = "Levels = " + numLevels.ToString();
            //labelNumLevels.Text = "Levels = " + Program.AutoScalerDefaults["translate_x_max"].ToString();
            statusBox.Text = "Next Step: Select the number of Glyphs at each level, and press the 'Do Levels' button.";
            int trow = tableLayoutPanel1.RowCount;
            tableLayoutPanel1.Controls.Clear();
            tableLayoutPanel1.RowCount = 0;
            goarounds++;
            int origGLCcount = glyphLevelControls.Count;
            if (origGLCcount > 0)
            {
                for (int rj = 0; rj < origGLCcount; rj++)
                {
                    glyphLevelControls.RemoveAt((origGLCcount - 1) - rj);
                }
            }
            
            for (int ri = trow - 1; ri == 0; ri--)
            {
                tableLayoutPanel1.Controls.RemoveAt(ri);
            }

            for (int ii = 0; ii < numLevels; ii++)
                {
                    groupBox2.Visible = true;
                    tableLayoutPanel1.Controls.Add(new Label() { Text = "Glyphs at Level[" + ii.ToString() +"]: "  , Anchor = AnchorStyles.Left, AutoSize = true, }, 0, ii );
                    //tableLayoutPanel1.RowCount++;
                    TextBox txtbx = new TextBox();
                    Label lvlLable = new Label();
                    lvlLable.Text = "0";
                    NumericUpDown lvl_UD = new NumericUpDown() { Name = "Level =" + ii.ToString() };
                    lvl_UD.Minimum = 0;
                    lvl_UD.Maximum = 16;
                    lvl_UD.Value = 0;
                    lvl_UD.Name = "L_" + ii.ToString();
                    lvl_UD.Tag  = "Level_" + ii.ToString();
                    txtbx.Text  = "Level =" + ii.ToString();
                    txtbx.Name  = "txtbx_Level" + ii.ToString();
                    txtbx.Tag   = "txtbx_Level" + ii.ToString();

                    //txtbx.Height = 20;
                    //txtbx.Width = 50;
                    TrackBar tBar = new TrackBar();
                    tBar.Name = "numGlyphs_at_Level_" + ii.ToString();
                    glyphLevelControls.Add(lvl_UD);
                    tableLayoutPanel1.Controls.Add(lvl_UD, 1, ii);
                    numberOfLevelsSet = true;
                    if (numberOfLevelsSet && numberOfGlyphsPerLevelsSet)
                    {
                        saveToolStripMenuItem.Enabled = true;
                        saveAsToolStripMenuItem.Enabled = true;
                    }
                }
            
        }

        private void trackBar1_TabIndexChanged(object sender, EventArgs e)
        {
            label1.Text = trackBar1.Value.ToString();
        }

        private void do_Levels_Click(object sender, EventArgs e)
        {
            int tti = 0;
            
            foreach (TabPage tpage in myTabList)
            {
                if ((!tpage.Name.Equals("tabStart")) || (! tpage.Name.Equals( "tab_L0") ) )
                    exTC1.HideTabPage(tpage);
                if (tti <= glyphLevelControls.Count+1)
                {
                    exTC1.ShowTabPage(tpage);
                }
                tti++;
            }

            String lstring, ostring ="";
            for (int gi = 0; gi < glyphLevelControls.Count; gi++)
            {
                lstring = "L_" + gi.ToString();
                ostring += lstring + "=" + tableLayoutPanel1.Controls[lstring].Text + ", ";
                // add a label to the appropriate tab
                TabPage ttemp = myTabDict[gi];
                Label lvlLabel = new Label();
                TableLayoutPanel elTabel = new TableLayoutPanel();
                lvlLabel.AutoSize = true;
                if (gi == 0 ){
                    lvlLabel.Text = "Level " + gi.ToString() + " Traditionaly Antz has ONE glyph at L0";
                    glyphsPerLevel[gi] = Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text);
                }
                else
                {
                    lvlLabel.Text = "Level " + gi.ToString() + " has " + tableLayoutPanel1.Controls[lstring].Text + " glyphs.";
                    glyphsPerLevel[gi] = Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text);
                }
                    
                
                ttemp.Controls.Add(lvlLabel);
                elTabel.AutoScroll = true;
                elTabel.AutoSize = true;
                elTabel.Location = new Point(5, 22);
                elTabel.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;
                elTabel.Name = "tabel_" + lstring;
                    
                if (lstring.Equals("L_0"))
                {   
                    elTabel.Controls.Add(new Label() { Text = " Glyphs # "     , Anchor = AnchorStyles.Left, Width = 80, }, 0, 0);
                    elTabel.Controls.Add(new Label() { Text = " X "            , Anchor = AnchorStyles.Left, Width = 80, }, 1, 0);
                    elTabel.Controls.Add(new Label() { Text = " Y "            , Anchor = AnchorStyles.Left, Width = 80, }, 2, 0);
                    elTabel.Controls.Add(new Label() { Text = " Z "            , Anchor = AnchorStyles.Left, Width = 80, }, 3, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale X"      , Anchor = AnchorStyles.Left, Width = 80, }, 4, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Y"      , Anchor = AnchorStyles.Left, Width = 80, }, 5, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Z"      , Anchor = AnchorStyles.Left, Width = 80, }, 6, 0);
                    elTabel.Controls.Add(new Label() { Text = " Geometry"     , Anchor = AnchorStyles.Left, Width = 80, }, 7, 0);
                    elTabel.Controls.Add(new Label() { Text = " Topology "     , Anchor = AnchorStyles.Left, Width = 80, }, 8, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Index"  , Anchor = AnchorStyles.Left, Width = 85, }, 9, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Red "    , Anchor = AnchorStyles.Left, Width = 80, }, 10, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Green "  , Anchor = AnchorStyles.Left, Width = 85, }, 11, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Blue  "  , Anchor = AnchorStyles.Left, Width = 80, }, 12, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Alpha " , Anchor = AnchorStyles.Left, Width = 85, }, 13, 0);
                    elTabel.Controls.Add(new Label() { Text = "ChannelS "      , Anchor = AnchorStyles.Left, Width = 80, }, 14, 0);
                    tableLayoutPanel1.Controls[lstring].Text = "1";
                    //for (int ii = 1; ii <= Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text); ii++)
                    for (int ii = 1; ii <= 1; ii++) // there can be only One Root Pin                    
                    {
                        string glyfNum = "_g_" + ii.ToString(); 
                        elTabel.Controls.Add(new Label()    { Text = " # "+ ii.ToString(), Anchor = AnchorStyles.Left, Width = 80, }, 0, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_X"         , DropDownStyle = ComboBoxStyle.DropDownList, DataSource=new BindingSource( DD_list_items, null), DisplayMember="Value", ValueMember="key", Anchor = AnchorStyles.Left, Width = 80, }, 1, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Y"         , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 2, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Z"         , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 3, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_x"   , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 4, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_y"   , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 5, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_z"   , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 6, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry"  , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 7, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Topology"  , DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DT_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 8, ii);
                        ComboBox CPcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorPalette", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80 };
                        ComboBox CIcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorIndex", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, };
                        FlowLayoutPanel FloBox = new FlowLayoutPanel () { Name = lstring + glyfNum + "_FlowBox", Width = 85, FlowDirection= System.Windows.Forms.FlowDirection.TopDown };
                        FloBox.Controls.Add(CPcobox);
                        FloBox.Controls.Add(CIcobox);
                        elTabel.Controls.Add(FloBox, 9, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_r", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 10, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_g", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 11, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_b", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 12, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_a", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 13, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Channel", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DC_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 14, ii);

                    }
                }
                else
                {
                    elTabel.Controls.Add(new Label() { Text = " Glyphs # "     , Anchor = AnchorStyles.Left, Width = 80, }, 0, 0);
                    elTabel.Controls.Add(new Label() { Text = " Geometry "     , Anchor = AnchorStyles.Left, Width = 80, }, 1, 0);
                    elTabel.Controls.Add(new Label() { Text = " Topology "     , Anchor = AnchorStyles.Left, Width = 80, }, 2, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Index "  , Anchor = AnchorStyles.Left, Width = 85, }, 3, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Red "    , Anchor = AnchorStyles.Left, Width = 80, }, 4, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Green "  , Anchor = AnchorStyles.Left, Width = 85, }, 5, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Blue "   , Anchor = AnchorStyles.Left, Width = 80, }, 6, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Alpha " , Anchor = AnchorStyles.Left, Width = 85, }, 7, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale X "      , Anchor = AnchorStyles.Left, Width = 80, }, 8, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Y "      , Anchor = AnchorStyles.Left, Width = 80, }, 9, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Z "      , Anchor = AnchorStyles.Left, Width = 80, },10, 0);
                    elTabel.Controls.Add(new Label() { Text = " Rotate  "      , Anchor = AnchorStyles.Left, Width = 80, },11, 0);
                    elTabel.Controls.Add(new Label() { Text = " Spin    "      , Anchor = AnchorStyles.Left, Width = 80, },12, 0);
                    elTabel.Controls.Add(new Label() { Text = " Orbit   "      , Anchor = AnchorStyles.Left, Width = 80, },13, 0);
                    elTabel.Controls.Add(new Label() { Text = " Channel "      , Anchor = AnchorStyles.Left, Width = 80, },14, 0);
                    for (int ii = 1; ii <= Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text); ii++)
                    {
                        string glyfNum = "_g_" + ii.ToString(); 
                        elTabel.Controls.Add(new Label()    { Text = " # " + ii.ToString()          ,           Anchor = AnchorStyles.Left, Width = 80, }, 0, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry",           DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 1, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Topology",           DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DT_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 2, ii);
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_ColorIndex",         DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 3, ii);
                        ComboBox CPcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorPalette", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80 };
                        ComboBox CIcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorIndex", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, };
                        FlowLayoutPanel FloBox = new FlowLayoutPanel() { Name = lstring + glyfNum + "_FlowBox", Width = 80, FlowDirection = System.Windows.Forms.FlowDirection.TopDown };
                        FloBox.Controls.Add(CPcobox);
                        FloBox.Controls.Add(CIcobox);
                        elTabel.Controls.Add(FloBox, 3, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_r",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 4, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_g",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 5, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_b",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 6, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_a",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 7, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_x",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 8, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_y",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 9, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_z",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, },10, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_X",                  DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, },11, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_translate_rate_y",   DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, },12, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_translate_rate_x",   DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, },13, ii);
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Channel",            DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DC_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, },14, ii);
                    }
                }
                
                ttemp.Controls.Add(elTabel);    
            }
            numberOfGlyphsPerLevelsSet = true;
            if (numberOfLevelsSet && numberOfGlyphsPerLevelsSet)
            {
                saveToolStripMenuItem.Enabled = true;
                saveAsToolStripMenuItem.Enabled = true;
            }
        }

        private void do_Levels_Map_Diff()
        {
            int tti = 0;
            int maxLevels = 0;
            this.SuspendLayout();
            for (int i = 0; i < 16; i++)
            {
                if ( glyphsPerLevel[i] !=0 ) maxLevels++;
            }
            foreach (TabPage tpage in myTabList)
            {
                if ((!tpage.Name.Equals("tabStart")) || (!tpage.Name.Equals("tab_L0")))
                    exTC1.HideTabPage(tpage);
                if (tti <= maxLevels + 1)
                {
                    exTC1.ShowTabPage(tpage);
                }
                tti++;
            }
            numLevels = maxLevels;
            String lstring = "";
            for (int gi = 0; gi < maxLevels; gi++) // TODO change this to come from analyze glyph results 
            {
                lstring = "L_" + gi.ToString();
                // ostring += lstring + "=" + tableLayoutPanel1.Controls[lstring].Text + ", ";
                // add a label to the appropriate tab
                TabPage ttemp = myTabDict[gi];
                Label lvlLabel = new Label();
                TableLayoutPanel elTabel = new TableLayoutPanel();
                lvlLabel.AutoSize = true;
                if (gi == 0)
                {
                    lvlLabel.Text = "Level " + gi.ToString() + " Traditionaly Antz has ONE glyph at L0";
                    glyphLevelControls.Add(lvlLabel);
                    // glyphsPerLevel[gi] = Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text);  // TODO
                }
                else
                {
                    lvlLabel.Text = "Level " + gi.ToString() + " has " + glyphsPerLevel[gi] + " glyphs.";
                    glyphLevelControls.Add(lvlLabel);
                    // glyphsPerLevel[gi] = Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text);  // TODO
                }


                ttemp.Controls.Add(lvlLabel);
                elTabel.AutoScroll = true;
                elTabel.AutoSize = true;
                elTabel.Location = new Point(5, 22);
                elTabel.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;
                elTabel.Name = "tabel_" + lstring;


                if (lstring.Equals("L_0"))
                {
                    elTabel.Controls.Add(new Label() { Text = " Glyphs # ", Anchor = AnchorStyles.Left, Width = 80, }, 0, 0);
                    elTabel.Controls.Add(new Label() { Text = " X ", Anchor = AnchorStyles.Left, Width = 80, }, 1, 0);
                    elTabel.Controls.Add(new Label() { Text = " Y ", Anchor = AnchorStyles.Left, Width = 80, }, 2, 0);
                    elTabel.Controls.Add(new Label() { Text = " Z ", Anchor = AnchorStyles.Left, Width = 80, }, 3, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale X", Anchor = AnchorStyles.Left, Width = 80, }, 4, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Y", Anchor = AnchorStyles.Left, Width = 80, }, 5, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Z", Anchor = AnchorStyles.Left, Width = 80, }, 6, 0);
                    elTabel.Controls.Add(new Label() { Text = " Ratio ", Anchor = AnchorStyles.Left, Width = 80, }, 7, 0);
                    elTabel.Controls.Add(new Label() { Text = " Geometry", Anchor = AnchorStyles.Left, Width = 80, }, 8, 0);
                    elTabel.Controls.Add(new Label() { Text = " Topology ", Anchor = AnchorStyles.Left, Width = 80, }, 9, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Index", Anchor = AnchorStyles.Left, Width = 85, }, 10, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Red ", Anchor = AnchorStyles.Left, Width = 80, }, 11, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Green ", Anchor = AnchorStyles.Left, Width = 85, }, 12, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Blue  ", Anchor = AnchorStyles.Left, Width = 80, }, 13, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Alpha ", Anchor = AnchorStyles.Left, Width = 85, }, 14, 0);
                    elTabel.Controls.Add(new Label() { Text = "ChannelS ", Anchor = AnchorStyles.Left, Width = 80, }, 15, 0);
                    // tableLayoutPanel1.Controls[lstring].Text = "1";
                    // for (int ii = 1; ii <= Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text); ii++)
                    for (int ii = 1; ii <= 1; ii++) // there can be only One Root Pin                    
                    {
                        string glyfNum = "_g_" + ii.ToString();
                        elTabel.Controls.Add(new Label() { Text = " # " + ii.ToString(), Anchor = AnchorStyles.None, Width = 80, }, 0, ii);
                        
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_X", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 1, ii);
                        ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_X", DD_list_items);
                        coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                        elTabel.Controls.Add(coBoxer, 1, ii);
                        
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Y", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 2, ii);
                        coBoxer = mkComboBox(lstring, glyfNum, "_Y", DD_list_items);
                        coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                        elTabel.Controls.Add(coBoxer, 2, ii);
                        
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Z", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 3, ii);
                        coBoxer = mkComboBox(lstring, glyfNum, "_Z", DD_list_items);
                        coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                        elTabel.Controls.Add(coBoxer, 3, ii);
                        
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_x", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 4, ii);
                        if (ampzarray[gi, ii - 1].isDiff_scale_x)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 4, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 4, ii);
                        }                       
                        
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_y", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 5, ii);
                        if (ampzarray[gi, ii - 1].isDiff_scale_y)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 5, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 5, ii);
                        }                       
                        
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_z", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 6, ii);
                        if (ampzarray[gi, ii - 1].isDiff_scale_z)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_z", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 6, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_scale_z", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 6, ii);
                        }
                        if (ampzarray[gi, ii - 1].isDiff_ratio)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_ratio", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 7, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_ratio", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 7, ii);
                        }         
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 7, ii);
                        if (ampzarray[gi, ii - 1].isDiff_geometry) {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_Geometry", DG_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 8, ii);
                        }
                        else {
                            coBoxer = mkComboBox(lstring, glyfNum, "_Geometry", DG_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 8, ii);
                        }
                         
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Topology", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DT_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80, }, 9, ii);


                        //ComboBox CPcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorPalette", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.None, Width = 80 };
                        //ComboBox CIcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorIndex", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.None, Width = 80, };
                        //FlowLayoutPanel FloBox = new FlowLayoutPanel() { Name = lstring + glyfNum + "_FlowBox", Width = 85, FlowDirection = System.Windows.Forms.FlowDirection.TopDown };
                        //FloBox.Controls.Add(CPcobox);
                        //FloBox.Controls.Add(CIcobox);
                        
                        
                        ComboBox CPcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorPalette", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80 }; //, Anchor = AnchorStyles.Top|AnchorStyles.Bottom,
                        ComboBox CIcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorIndex", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80, };
                        FlowLayoutPanel FloBox = new FlowLayoutPanel() { Name = lstring + glyfNum + "_FlowBox", Width = 85, FlowDirection = System.Windows.Forms.FlowDirection.TopDown };
                        FloBox.Controls.Add(CPcobox);
                        FloBox.Controls.Add(CIcobox);
                        elTabel.Controls.Add(FloBox, 10, ii);
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_r", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 11, ii);
                        if (ampzarray[gi, ii - 1].isDiff_color_r)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_r", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 11, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_r", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 11, ii);
                        }
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_g", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 12, ii);
                        if (ampzarray[gi, ii - 1].isDiff_color_g)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_g", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 12, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_g", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 12, ii);
                        }
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_b", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 13, ii);
                        if (ampzarray[gi, ii - 1].isDiff_color_b)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_b", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 13, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_b", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 13, ii);
                        }                        
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_a", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 14, ii);
                        if (ampzarray[gi, ii - 1].isDiff_color_a)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_a", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 14, ii);
                        }
                        else
                        {
                            coBoxer = mkComboBox(lstring, glyfNum, "_color_a", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 14, ii);
                        }
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Channel", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DC_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 15, ii);
                        
                    }
                }
                else  // LEVEL 1 and Above
                {
                    elTabel.Controls.Add(new Label() { Text = " Glyphs # ", Anchor = AnchorStyles.Left, Width = 80, }, 0, 0);
                    elTabel.Controls.Add(new Label() { Text = " Geometry ", Anchor = AnchorStyles.Left, Width = 80, }, 1, 0);
                    elTabel.Controls.Add(new Label() { Text = " Topology ", Anchor = AnchorStyles.Left, Width = 80, }, 2, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Index ", Anchor = AnchorStyles.Left, Width = 85, }, 3, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Red ", Anchor = AnchorStyles.Left, Width = 80, }, 4, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Green ", Anchor = AnchorStyles.Left, Width = 85, }, 5, 0);
                    elTabel.Controls.Add(new Label() { Text = " Color Blue ", Anchor = AnchorStyles.Left, Width = 80, }, 6, 0);
                    elTabel.Controls.Add(new Label() { Text = "Color Alpha ", Anchor = AnchorStyles.Left, Width = 85, }, 7, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale X ", Anchor = AnchorStyles.Left }, 8, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Y ", Anchor = AnchorStyles.Left, Width = 80, }, 9, 0);
                    elTabel.Controls.Add(new Label() { Text = " Scale Z ", Anchor = AnchorStyles.Left, Width = 80, }, 10, 0);
                    elTabel.Controls.Add(new Label() { Text = " Ratio ", Anchor = AnchorStyles.Left, Width = 80, }, 11, 0);
                    elTabel.Controls.Add(new Label() { Text = " Rotate  ", Anchor = AnchorStyles.Left, Width = 80, }, 12, 0);
                    elTabel.Controls.Add(new Label() { Text = " Spin    ", Anchor = AnchorStyles.Left, Width = 80, }, 13, 0);
                    elTabel.Controls.Add(new Label() { Text = " Orbit   ", Anchor = AnchorStyles.Left, Width = 80, }, 14, 0);
                    elTabel.Controls.Add(new Label() { Text = " Channel ", Anchor = AnchorStyles.Left, Width = 80, }, 15, 0);
                    //  for (int ii = 1; ii <= Convert.ToInt16(tableLayoutPanel1.Controls[lstring].Text); ii++)
                    for (int ii = 1; ii <= glyphsPerLevel[gi]; ii++)
                    {
                        string glyfNum = "_g_" + ii.ToString();
                        elTabel.Controls.Add(new Label() { Text = " # " + ii.ToString(), Anchor = AnchorStyles.Left, Width = 80, }, 0, ii);
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 1, ii);
                        if (ampzarray[gi, ii ].isDiff_geometry)     {
                            //backgroundColorPanel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80, Top=(int)((backgroundColorPanel.Height/2)-13)}); // ,
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_Geometry", DG_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 1, ii);
                        }
                        else   {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_Geometry", DG_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 1, ii);
                        } 
                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Topology", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DT_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 2, ii);
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_ColorIndex",         DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 3, ii);
                        ComboBox CPcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorPalette", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(Program.DP_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.None, Width = 80 };
                        ComboBox CIcobox = new ComboBox() { Name = lstring + glyfNum + "_ColorIndex", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.None, Width = 80, };
                        FlowLayoutPanel FloBox = new FlowLayoutPanel() { Name = lstring + glyfNum + "_FlowBox", Width = 85,  FlowDirection = System.Windows.Forms.FlowDirection.TopDown };
                        FloBox.AutoSize = true;
                        FloBox.Controls.Add(CPcobox);
                        FloBox.Controls.Add(CIcobox);
                         elTabel.Controls.Add(FloBox, 3, ii);
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_r", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 4, ii);
                        if (ampzarray[gi, ii].isDiff_color_r)
                        {
                            //backgroundColorPanel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80, Top=(int)((backgroundColorPanel.Height/2)-13)}); // ,
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_r", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 4, ii);
                        }
                        else
                        {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_r", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 4, ii);
                        } 
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_g", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 5, ii);
                        if (ampzarray[gi, ii].isDiff_color_g)
                        {
                            //backgroundColorPanel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Width = 80, Top=(int)((backgroundColorPanel.Height/2)-13)}); // ,
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_g", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 5, ii);
                        }
                        else
                        {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_g", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 5, ii);
                        } 
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_b", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 6, ii);
                        if (ampzarray[gi, ii].isDiff_color_b)   {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_b", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 6, ii);
                        }
                        else   {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_b", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 6, ii);
                        } 
                        // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_color_a", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 7, ii);
                        if (ampzarray[gi, ii].isDiff_color_a)  {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_a", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 7, ii);
                        }
                        else   {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_color_a", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 7, ii);
                        } 
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_x", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 8, ii);
                        if (ampzarray[gi, ii].isDiff_scale_x) {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer); 
                            elTabel.Controls.Add(backgroundColorPanel, 8, ii);
                        }
                        else {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 8, ii);
                        }
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_y", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 9, ii);
                        if (ampzarray[gi, ii].isDiff_scale_y)  {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 9, ii);
                        }
                        else  {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 9, ii);
                        }
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_scale_z", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 10, ii);
                        if (ampzarray[gi, ii].isDiff_scale_z)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_z", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 10, ii);
                        }
                        else
                        {
                            // elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Geometry", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DG_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.None, Width = 80 }, 1, ii); //,
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_scale_z", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 10, ii);
                        }
                        if (ampzarray[gi, ii].isDiff_ratio)  {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_ratio", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 11, ii);
                        }
                        else  {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_ratio", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 11, ii);
                        } 
                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_translate_x", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 11, ii);
                        //12
                        if (ampzarray[gi, ii].isDiff_translate_x)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_X", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 12, ii);
                        }
                        else
                        {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_X", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 12, ii);
                        } 

                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_translate_rate_y", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 12, ii);
                        //13
                        if (ampzarray[gi, ii].isDiff_translate_rate_y)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_translate_rate_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 13, ii);
                        }
                        else
                        {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_translate_rate_y", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 13, ii);
                        } 

                        //elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_translate_rate_x", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DD_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 13, ii);
                        //14
                        if (ampzarray[gi, ii].isDiff_translate_rate_x)
                        {
                            Panel backgroundColorPanel = newColorPanel( lstring, glyfNum);
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_translate_rate_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            coBoxer.Top = (int)(backgroundColorPanel.Height / 2 - 13);
                            backgroundColorPanel.Controls.Add(coBoxer);
                            elTabel.Controls.Add(backgroundColorPanel, 14, ii);
                        }
                        else
                        {
                            ComboBox coBoxer = mkComboBox(lstring, glyfNum, "_translate_rate_x", DD_list_items);
                            coBoxer.SelectionChangeCommitted += new System.EventHandler(this.onDropDownChange);
                            elTabel.Controls.Add(coBoxer, 14, ii);
                        } 

                        elTabel.Controls.Add(new ComboBox() { Name = lstring + glyfNum + "_Channel", DropDownStyle = ComboBoxStyle.DropDownList, DataSource = new BindingSource(DC_list_items, null), DisplayMember = "Value", ValueMember = "key", Anchor = AnchorStyles.Left, Width = 80, }, 15, ii);
                    }
                }

                ttemp.Controls.Add(elTabel);
            }
            this.ResumeLayout(); 
            numberOfGlyphsPerLevelsSet = true;
            //numLevels;
            if (numberOfLevelsSet && numberOfGlyphsPerLevelsSet)
            {
                saveToolStripMenuItem.Enabled = true;
                saveAsToolStripMenuItem.Enabled = true;
            }
        }

        private ComboBox mkComboBox(String lstring, string glyfNum, string nameStr, Dictionary<string, string> DList)
        {
            string nString = lstring + glyfNum + nameStr;
            return new ComboBox() { Name = nString.ToString(), DropDownStyle = ComboBoxStyle.DropDownList, 
                DataSource = new BindingSource(DList, null), DisplayMember = "Value", ValueMember = "key" 
                                ,   Anchor = AnchorStyles.Left//  | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom
            };
        }

        private Panel newColorPanel(string lstring, string glyfNum )
        {
            Panel backgroundColorPanel = new Panel();
            backgroundColorPanel.Name = lstring + glyfNum + "_colorPanel";
            backgroundColorPanel.BackColor = Color.LightGreen;
            //backgroundColorPanel.Height = 50;
            //backgroundColorPanel.Width  = 90;
            backgroundColorPanel.Dock = DockStyle.Fill;
            backgroundColorPanel.Margin = new Padding(0);
            backgroundColorPanel.Anchor = AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom; // ((System.Windows.Forms.AnchorStyles)(System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left));
            backgroundColorPanel.AutoSize = true;
            //backgroundColorPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            return backgroundColorPanel;
        }
        
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Really Quit?", "Exit", MessageBoxButtons.OKCancel) == DialogResult.OK)
            {
                Application.Exit();
            }
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutForm frm = new AboutForm();
            frm.ShowDialog();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {// When user clicks button, show the dialog.
            saveFileDialog1.ShowDialog();
        }

        private void saveFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            statusBox.Text = "Status: now Saving the Antz File...";
            var file = new FileInfo(saveFileDialog1.FileName);
            int id;
            bool skip_this_data_point = false;
            Dictionary<string, string> thisRow = new Dictionary<string, string>();
            Dictionary<string, string> pastRow = new Dictionary<string, string>();
            string saveFileName = saveFileDialog1.FileName;
            string saveChanName = file.DirectoryName + "\\" + file.Name + "-ANTzCh0001.csv";
            
            Int32 lvl = 0, glf = 1;
            double MinoutA = -90.0;
            double MaxoutB =  90.0;
            double DataMin =  -1.0;
            double DataMax =   1.0;

            // First create a global ampz array to save the users selection into 
            // Refactor: this could be a method
            bool wf;
            if (Program.DiffGlyphMappingWorkflow)
            {
                wf = true;
                for (lvl = 0; lvl < glyphLevelControls.Count; lvl++) // loop through each level
                {
                    string lstring = "L_" + lvl.ToString();
                    TabPage levelTabPage = myTabDict[lvl];
                    // loop through all glyphs at that level....
                    string strLvl = "L_" + lvl;
                    // Console.WriteLine("__________________________ Level = " + lvl.ToString() + " __________________________");
                    TabPage lvltapge = myTabDict[lvl];
                    foreach (TableLayoutPanel tPan in lvltapge.Controls.OfType<TableLayoutPanel>())
                    {
                        //   for (glf = 0; glf <= Convert.ToInt16(tableLayoutPanel1.Controls[strLvl].Text); glf++)
                        // 7602164191
                        for (glf = 1; glf <= glyphsPerLevel[lvl]; glf++)
                            {
                            // Console.WriteLine("    ______________________ Glyph = " + glf.ToString() + " ______________________ ");
                            string ssrch = "_g_" + (glf).ToString();
                            foreach (ComboBox coBox in tPan.Controls.OfType<ComboBox>())
                            //foreach (ComboBox coBox in tPan.Controls.   )
                            { // Stuff the gui controlBox values into the global ampzarray
                                if (coBox.Name.Contains(ssrch))
                                {
                                    if (coBox.Name.Contains("_X")) { ampzarray[lvl, glf].translate_x = coBox.Text; }
                                    if (coBox.Name.Contains("_Y")) { ampzarray[lvl, glf].translate_y = coBox.Text; }
                                    if (coBox.Name.Contains("_Z")) { ampzarray[lvl, glf].translate_z = coBox.Text; }
                                    if (coBox.Name.Contains("_Geometry")) { ampzarray[lvl, glf].geometry = coBox.Text; }
                                    if (coBox.Name.Contains("_Topology")) { ampzarray[lvl, glf].topology = coBox.Text; }
                                    if (coBox.Name.Contains("_ColorIndex")) { ampzarray[lvl, glf].color_index = coBox.Text; }
                                    if (coBox.Name.Contains("_color_r")) { ampzarray[lvl, glf].color_r = coBox.Text; }
                                    if (coBox.Name.Contains("_color_g")) { ampzarray[lvl, glf].color_g = coBox.Text; }
                                    if (coBox.Name.Contains("_color_b")) { ampzarray[lvl, glf].color_b = coBox.Text; }
                                    if (coBox.Name.Contains("_color_a")) { ampzarray[lvl, glf].color_a = coBox.Text; }
                                    if (coBox.Name.Contains("_record_id")) { ampzarray[lvl, glf].record_id = coBox.Text; }
                                    if (coBox.Name.Contains("_translate_rate_x")) { ampzarray[lvl, glf].translate_rate_x = coBox.Text; }
                                    if (coBox.Name.Contains("_translate_rate_y")) { ampzarray[lvl, glf].translate_rate_y = coBox.Text; }
                                    if (coBox.Name.Contains("_scale_x")) { ampzarray[lvl, glf].scale_x = coBox.Text; }
                                    if (coBox.Name.Contains("_scale_y")) { ampzarray[lvl, glf].scale_y = coBox.Text; }
                                    if (coBox.Name.Contains("_scale_z")) { ampzarray[lvl, glf].scale_z = coBox.Text; }
                                    if (coBox.Name.Contains("_ratio")) { ampzarray[lvl, glf].ratio = coBox.Text; }
                                    if (coBox.Name.Contains("_Channel")) { ampzarray[lvl, glf].channel = coBox.Text; }
                                }
                            } // for each comboBox on the tab   
                            foreach (FlowLayoutPanel FloBox in tPan.Controls.OfType<FlowLayoutPanel>())
                            {
                                if (FloBox.Name.Contains(ssrch))
                                {
                                    foreach (ComboBox coBox in FloBox.Controls.OfType<ComboBox>())
                                    {
                                        if (coBox.Name.Contains("_ColorIndex")) { ampzarray[lvl, glf].color_index = coBox.Text; }
                                        if (coBox.Name.Contains("_ColorPalette")) { ampzarray[lvl, glf].color_palette = coBox.Text; }
                                    }
                                }

                            }
                            foreach (Panel cPan in tPan.Controls.OfType<Panel>())
                            {
                                if (cPan.Name.Contains(ssrch))
                                {
                                    foreach (ComboBox coBox in cPan.Controls.OfType<ComboBox>())
                                    {
                                        if (coBox.Name.Contains("_X")) { ampzarray[lvl, glf].translate_x = coBox.Text; }
                                        if (coBox.Name.Contains("_Y")) { ampzarray[lvl, glf].translate_y = coBox.Text; }
                                        if (coBox.Name.Contains("_Z")) { ampzarray[lvl, glf].translate_z = coBox.Text; }
                                        if (coBox.Name.Contains("_Geometry")) { ampzarray[lvl, glf].geometry = coBox.Text; }
                                        if (coBox.Name.Contains("_Topology")) { ampzarray[lvl, glf].topology = coBox.Text; }
                                        if (coBox.Name.Contains("_ColorIndex")) { ampzarray[lvl, glf].color_index = coBox.Text; }
                                        if (coBox.Name.Contains("_color_r")) { ampzarray[lvl, glf].color_r = coBox.Text; }
                                        if (coBox.Name.Contains("_color_g")) { ampzarray[lvl, glf].color_g = coBox.Text; }
                                        if (coBox.Name.Contains("_color_b")) { ampzarray[lvl, glf].color_b = coBox.Text; }
                                        if (coBox.Name.Contains("_color_a")) { ampzarray[lvl, glf].color_a = coBox.Text; }
                                        if (coBox.Name.Contains("_record_id")) { ampzarray[lvl, glf].record_id = coBox.Text; }
                                        if (coBox.Name.Contains("_translate_rate_x")) { ampzarray[lvl, glf].translate_rate_x = coBox.Text; }
                                        if (coBox.Name.Contains("_translate_rate_y")) { ampzarray[lvl, glf].translate_rate_y = coBox.Text; }
                                        if (coBox.Name.Contains("_scale_x")) { ampzarray[lvl, glf].scale_x = coBox.Text; }
                                        if (coBox.Name.Contains("_scale_y")) { ampzarray[lvl, glf].scale_y = coBox.Text; }
                                        if (coBox.Name.Contains("_scale_z")) { ampzarray[lvl, glf].scale_z = coBox.Text; }
                                        if (coBox.Name.Contains("_ratio")) { ampzarray[lvl, glf].ratio = coBox.Text; }
                                        if (coBox.Name.Contains("_Channel")) { ampzarray[lvl, glf].channel = coBox.Text; }
                                    }
                                }

                            }
                        } // for gly=0..Number of glyphs
                    }// for each control in the selected tab
                } // for lvl = 0 .. number of levels
            }
            else
            { // not doing the glyphdiff workflow..
                wf = false;
                for (lvl = 0; lvl < glyphLevelControls.Count; lvl++) // loop through each level
            {
                string lstring = "L_" + lvl.ToString();
                TabPage levelTabPage = myTabDict[lvl];
                // loop through all glyphs at that level....
                string strLvl = "L_" + lvl;
                //Console.WriteLine("__________________________ Level = " + lvl.ToString() + " __________________________");
                TabPage lvltapge = myTabDict[lvl];
                foreach (TableLayoutPanel tPan in lvltapge.Controls.OfType<TableLayoutPanel>())
                {
                    for (glf = 0; glf <= Convert.ToInt16(tableLayoutPanel1.Controls[strLvl].Text); glf++)
                    {
                        //Console.WriteLine("    ______________________ Glyph = " + glf.ToString() + " ______________________ ");
                        string ssrch = "_g_" + (glf).ToString();
                        foreach (ComboBox coBox in tPan.Controls.OfType<ComboBox>())
                        //foreach (ComboBox coBox in tPan.Controls.   )
                        { // Stuff the gui controlBox values into the global ampzarray
                            if (coBox.Name.Contains(ssrch))
                            {
                                if (coBox.Name.Contains("_X")) { ampzarray[lvl, glf].translate_x = coBox.Text; }
                                if (coBox.Name.Contains("_Y")) { ampzarray[lvl, glf].translate_y = coBox.Text; }
                                if (coBox.Name.Contains("_Z")) { ampzarray[lvl, glf].translate_z = coBox.Text; }
                                if (coBox.Name.Contains("_Geometry")) { ampzarray[lvl, glf].geometry = coBox.Text; }
                                if (coBox.Name.Contains("_Topology")) { ampzarray[lvl, glf].topology = coBox.Text; }
                                if (coBox.Name.Contains("_ColorIndex")) { ampzarray[lvl, glf].color_index = coBox.Text; }
                                if (coBox.Name.Contains("_color_r")) { ampzarray[lvl, glf].color_r = coBox.Text; }
                                if (coBox.Name.Contains("_color_g")) { ampzarray[lvl, glf].color_g = coBox.Text; }
                                if (coBox.Name.Contains("_color_b")) { ampzarray[lvl, glf].color_b = coBox.Text; }
                                if (coBox.Name.Contains("_color_a")) { ampzarray[lvl, glf].color_a = coBox.Text; }
                                if (coBox.Name.Contains("_record_id")) { ampzarray[lvl, glf].record_id = coBox.Text; }
                                if (coBox.Name.Contains("_translate_rate_x")) { ampzarray[lvl, glf].translate_rate_x = coBox.Text; }
                                if (coBox.Name.Contains("_translate_rate_y")) { ampzarray[lvl, glf].translate_rate_y = coBox.Text; }
                                if (coBox.Name.Contains("_scale_x")) { ampzarray[lvl, glf].scale_x = coBox.Text; }
                                if (coBox.Name.Contains("_scale_y")) { ampzarray[lvl, glf].scale_y = coBox.Text; }
                                if (coBox.Name.Contains("_scale_z")) { ampzarray[lvl, glf].scale_z = coBox.Text; }
                                if (coBox.Name.Contains("_ratio")) { ampzarray[lvl, glf].ratio = coBox.Text; }
                                if (coBox.Name.Contains("_Channel")) { ampzarray[lvl, glf].channel = coBox.Text; }
                            }
                        } // for each comboBox on the tab   
                        foreach (FlowLayoutPanel FloBox in tPan.Controls.OfType<FlowLayoutPanel>())
                        {
                            if (FloBox.Name.Contains(ssrch))
                            {
                                foreach (ComboBox coBox in FloBox.Controls.OfType<ComboBox>())
                                {
                                    if (coBox.Name.Contains("_ColorIndex")) { ampzarray[lvl, glf].color_index = coBox.Text; }
                                    if (coBox.Name.Contains("_ColorPalette")) { ampzarray[lvl, glf].color_palette = coBox.Text; }
                                }
                            }

                        }
                    } // for gly=0..Number of glyphs
                }// for each control in the selected tab
            } // for lvl = 0 .. number of levels
            }
            //Console.WriteLine("wwwfff =" + wf.ToString());

            
            /*
                * Psuedo Code....
                * - select output file via SaveFile Dialogue
                * - open csv file, rewind to start
                * - write antz header to output file.
                * - read a row from the csv input file
                * - while ! end of rows
                * -   for each level
                * -     for each glyph
                * -       write antz row to output file
                * 
                */


            id = 0;
            rownum = 22;
            using (CsvReader csvResult = new CsvReader(new StreamReader(inFile)))
            {
                using ( swchan = new StreamWriter(saveChanName) )
                using (     sw = new StreamWriter(saveFileName) )
                {
                    //csvResult.Read();
                    //string[] headers = csvResult.FieldHeaders;
                    //Console.WriteLine(doHeader());
                    sw.WriteLine(doHeader());
                    swchan.WriteLine("cycleCount, ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8");
                    while (csvResult.Read())
                    {   // 
                        string[] headers = csvResult.FieldHeaders;
                        for (int i = 0; i < csvResult.FieldHeaders.Count(); i++)
                        {
                            if (!String.IsNullOrEmpty(dgv1.Rows[0].Cells[headers[i]].Value.ToString()))
                            {
                                DataMin = (double)Convert.ToDouble((dgv1.Rows[0].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[1].Cells[headers[i]].Value.ToString()))
                            {
                                DataMax = (double)Convert.ToDouble((dgv1.Rows[1].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[4].Cells[headers[i]].Value.ToString()))
                            {
                                MinoutA = (double)Convert.ToDouble((dgv1.Rows[4].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[5].Cells[headers[i]].Value.ToString()))
                            {
                                MaxoutB = (double)Convert.ToDouble((dgv1.Rows[5].Cells[headers[i]].Value.ToString()));
                            }
                            if ( (DataMin != MinoutA) || (DataMax != MaxoutB) )
                            {   // Autoscale Output Value
                                thisRow[headers[i]] = ((((MaxoutB - MinoutA) * (Convert.ToDouble(csvResult[i]) - DataMin)) / (DataMax - DataMin)) + MinoutA).ToString();
                            }
                            else
                                if (cbRangeData.Checked)
                                {
                                   // thisRow[headers[i]] = ((((diRa - MinoutA) * (Convert.ToDouble(csvResult[i]) - DataMin)) / (DataMax - DataMin)) + MinoutA).ToString();

                                }
                                else
                                {   // No AutoScale Output Value
                                    thisRow[headers[i]] = csvResult[i]; 
                                }
                            if ( (  Convert.ToDouble(csvResult[i]) < Convert.ToDouble(dgv1.Rows[6].Cells[headers[i]].Value)  )    // clip min out
                                 ||
                                 (  Convert.ToDouble(csvResult[i]) > Convert.ToDouble(dgv1.Rows[7].Cells[headers[i]].Value)  ) )  // Clip Max Out
                            {
                                skip_this_data_point = true;
                            }
                        }
                        if (skip_this_data_point != true)
                        {

                        
                            if ((id % nudSkipFactor.Value) == 0)
                            {
                                // check each row to see if it is within clipMinout and clipMaxout range
                                doGlyph(id, 0, 1, thisRow, pastRow);
                                for (int i = 0; i < csvResult.FieldHeaders.Count(); i++)
                                {
                                    pastRow[headers[i]] = thisRow[headers[i]];
                                }
                            }
                            id++;
                        }
                        skip_this_data_point = false;
                    }
                    if (cbAddTraillingGlyph.Checked == true)
                    {
                        // Refactor this
                    }
                    sw.Close(); 
                    swchan.Close();
                }
            }
            statusBox.Text = "Status: Successfully saved file: " + saveFileName + " id = " +id;
        } // saveFileDialog1_FileOk
        
        public void doGlyph(int parent_id, int branch_level, int kid_number, Dictionary<string, string> thisRow, Dictionary<string, string> pastRow)
        {
            int MAXD = numLevels ;     // SHP
            int child_count, current_row;
            double translate_x_val = 1.0, translate_y_val = 0.0, translate_z_val = 0.0;
            double translate_rate_x_val = 0.0, translate_rate_y_val = 0.0;
            double scale_x_val = 1.0, scale_y_val = 1.0, scale_z_val = 1.0, ratio_val = 0.1;
            int geom_val = 7, topo_val = 3, color_r_val = 05, color_g_val = 50, color_b_val = 255, color_a_val = 255,  ch_input_id = 0;
            double ch4 = 1.0, ch5 = 1.0, ch6 = 1.0, ch7 = 1.0, ch8 = 1.0, deg2rad = 0.01745329251994329576923690768489;
            Double E =0, F =0;
            // double AA, BB, CC, DD;
            current_row = rownum;
            // color_index_val = 1;
            //Console.WriteLine("doGlyph: parent_id: {0}, branch_level: {1}, kid_number: {2}, MaxD: {3}", parent_id,  branch_level, kid_number, MAXD);
            //Console.WriteLine("       : child_count: {0}, glyphsPerLevel[BL] {1}: , glyphsPerLevel[BL+1] {2}", child_count, glyphsPerLevel[branch_level], glyphsPerLevel[branch_level + 1]);
            if (branch_level == 0)  // root glyph
            { // All 15/16 user selectable data are mapped onto glyph params here
                geom_val = 16; topo_val = 5;
                if ((ampzarray[branch_level, 1].translate_x != null) && (ampzarray[branch_level, 1].translate_x != ""))
                {
                    translate_x_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].translate_x]);
                    //  Y = (X-A)/(B-A) * (D-C) + C 
                    //AA = (double)Convert.ToDouble((dgv1.Rows[0].Cells[headers[thisRow[ampzarray[branch_level, 1].translate_x]]].Value.ToString()));
                    //translate_x_val = ( ( Convert.ToDouble( thisRow[ampzarray[branch_level, 1].translate_x]  )- AA) / ( BB - AA)   )  * ( DD - CC ) + CC;
                }
                if ((ampzarray[branch_level, 1].translate_y != null)        && (ampzarray[branch_level, 1].translate_y != ""))      translate_y_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].translate_y]);
                if ((ampzarray[branch_level, 1].translate_z != null)        && (ampzarray[branch_level, 1].translate_z != ""))      translate_z_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].translate_z]);
                if ((ampzarray[branch_level, 1].ratio != null)              && (ampzarray[branch_level, 1].ratio != ""))            ratio_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].ratio]);
                if ((ampzarray[branch_level, 1].scale_x != null)            && (ampzarray[branch_level, 1].scale_x != ""))          scale_x_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].scale_x]);
                if ((ampzarray[branch_level, 1].scale_y != null)            && (ampzarray[branch_level, 1].scale_y != ""))          scale_y_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].scale_y]);
                if ((ampzarray[branch_level, 1].scale_z != null)            && (ampzarray[branch_level, 1].scale_z != ""))          scale_z_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].scale_z]);
                if ((ampzarray[branch_level, 1].geometry != null)           && (ampzarray[branch_level, 1].geometry != ""))
                {
                    switch (ampzarray[branch_level, 1].geometry)
                    {
                        case "Toroid":
                            geom_val = 7;
                            break;
                        case "Toroid Wire":
                            geom_val = 6;
                            break;
                        case "Cube":
                            geom_val = 1;
                            break;
                        case "Cube Wire":
                            geom_val = 0;
                            break;
                        case "Sphere":
                            geom_val = 3;
                            break;
                        case "Sphere Wire":
                            geom_val = 2;
                            break;
                        case "Cone":
                            geom_val = 5;
                            break;
                        case "Cone Wire":
                            geom_val = 4;
                            break;
                        case "Dodecahedron":
                            geom_val = 9;
                            break;
                        case "Dodecahedron Wire":
                            geom_val = 8;
                            break;
                        case "Octahedron":
                            geom_val = 11;
                            break;
                        case "Octahedron Wire":
                            geom_val = 10;
                            break;
                        case "Tetrahedron":
                            geom_val = 13;
                            break;
                        case "Tetrahedron Wire":
                            geom_val = 12;
                            break;
                        case "Icosahedron":
                            geom_val = 15;
                            break;
                        case "Icosahedron Wire":
                            geom_val = 14;
                            break;
                        case "Pin":
                            geom_val = 16;
                            break;
                        case "Pin Wire":
                            geom_val = 17;
                            break;
                        case "Cylinder":
                            geom_val = 19;
                            break;
                        case "Cylinder Wire":
                            geom_val = 18;
                            break;
                        case "Teapot":
                            geom_val = 20;
                            break;
                        case "Teapot Wire":
                            geom_val = 21;
                            break;
                        default:
                            geom_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].geometry]);
                            break;
                    }
                    
                }
                if ((ampzarray[branch_level, 1].topology != null) && (ampzarray[branch_level, 1].topology != ""))
                {
                    switch (ampzarray[branch_level, 1].topology)
                    {
                        case "Cube":
                            topo_val = 1;
                            break;
                        case "Sphere":
                            topo_val = 2;
                            break;
                        case "Torus":
                            topo_val = 3;
                            break;
                        case "Cylinder":
                            topo_val = 4;
                            break;
                        case "Pin":
                            topo_val = 5;
                            break;
                        case "Rod":
                            topo_val = 6;
                            break;
                        case "Point":
                            topo_val = 7;
                            break;
                        default:
                            topo_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].topology]);
                            break;
                    }
                } 
                if ((ampzarray[branch_level, 1].color_a != null)            && (ampzarray[branch_level, 1].color_a != ""))          color_a_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].color_a]);
                if ((ampzarray[branch_level, 1].color_r != null)            && (ampzarray[branch_level, 1].color_r != ""))          color_r_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].color_r]);
                if ((ampzarray[branch_level, 1].color_g != null)            && (ampzarray[branch_level, 1].color_g != ""))          color_g_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].color_g]);
                if ((ampzarray[branch_level, 1].color_b != null)            && (ampzarray[branch_level, 1].color_b != ""))          color_b_val = Convert.ToInt16(thisRow[ampzarray[branch_level, 1].color_b]);
                if ((ampzarray[branch_level, 1].color_index != null) && (ampzarray[branch_level, 1].color_index != ""))             //color_index_val = Convert.ToInt16(inRow[ampzarray[branch_level, 1].color_index]);
                {  //  Special color Palette handling
                    if (Program.DP_list_items.ContainsKey(ampzarray[branch_level, 1].color_palette))
                    { // keep functionality to read in hexidecimal ARGB formatted numbers
                        string hexString = thisRow[ampzarray[branch_level, 1].color_index];
                        if (System.Text.RegularExpressions.Regex.IsMatch(hexString, @"[#]([0-9]|[a-f]|[A-F]){8}\b"))
                        { // found #AARRGGBB
                            color_a_val = int.Parse(hexString.Substring(1, 2), NumberStyles.HexNumber);
                            color_r_val = int.Parse(hexString.Substring(3, 2), NumberStyles.HexNumber);
                            color_g_val = int.Parse(hexString.Substring(5, 2), NumberStyles.HexNumber);
                            color_b_val = int.Parse(hexString.Substring(7, 2), NumberStyles.HexNumber);
                        }
                        else if (System.Text.RegularExpressions.Regex.IsMatch(hexString, @"[#]([0-9]|[a-f]|[A-F]){6}\b"))
                        {  // found #RRGGBB
                            color_a_val = int.Parse(hexString.Substring(1, 2), NumberStyles.HexNumber);
                            color_r_val = int.Parse(hexString.Substring(3, 2), NumberStyles.HexNumber);
                            color_g_val = int.Parse(hexString.Substring(5, 2), NumberStyles.HexNumber);
                            color_b_val = int.Parse(hexString.Substring(7, 2), NumberStyles.HexNumber);
                        }
                        // not a haxidecimal number
                        // get palette lo & hi number and color then linear interpolate rgb
                        // this can be made more efficient later.
                        double datum = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].color_index]);
                        Color loColr = Program.PaletteList[ampzarray[branch_level, 1].color_palette].lowColor;
                        Color hiColr = Program.PaletteList[ampzarray[branch_level, 1].color_palette].hiColor;
                        Int32 Ar = loColr.R; Int32 Br = hiColr.R;
                        Int32 Ag = loColr.G; Int32 Bg = hiColr.G;
                        Int32 Ab = loColr.B; Int32 Bb = hiColr.B;
                        double C = 0; // Program.PaletteList[ampzarray[branch_level, 1].color_palette].lowNumber;
                        double D = 255; // Program.PaletteList[ampzarray[branch_level, 1].color_palette].hiNumber;
                        // need to scale datum to range
                        // Refactor e f

                        //E = Program.colData[ampzarray[branch_level, 1].color_index].colMin;
                        //F = Program.colData[ampzarray[branch_level, 1].color_index].colMax;

                        E = (Program.colData[ampzarray[branch_level, 1].color_index].colMin != Program.colData[ampzarray[branch_level, 1].color_index].colMinout)
                          ?  Program.colData[ampzarray[branch_level, 1].color_index].colMinout
                          :  Program.colData[ampzarray[branch_level, 1].color_index].colMin;
                        F = (Program.colData[ampzarray[branch_level, 1].color_index].colMax != Program.colData[ampzarray[branch_level, 1].color_index].colMaxout)
                          ?  Program.colData[ampzarray[branch_level, 1].color_index].colMaxout
                          :  Program.colData[ampzarray[branch_level, 1].color_index].colMax;


                        double p = (datum - C) / (double)(D - C);
                        //double p = (datum - E) / (double)(F - E);
                        //double p = (F - E) != 0 ? (datum - E) / (double)(F - E) : (datum - E);
                        Int32 rval = interpolate(Ar, Br, p);
                        color_r_val = rval;
                        //Console.WriteLine("Ar = {0} Br = {1} C = {2} D = {3} Datum = {4} p = {5} rval = {6}", Ar, Br, C, D, datum, p, rval);    
                            
                        color_g_val = interpolate(Ag, Bg, p);
                        color_b_val = interpolate(Ab, Bb, p);
                        //color_r_val = (int)(C * ( 1 -  ( (datum - Ar) / ( System.Math.Abs(Br - Ar) ) ) ) + ( D * ( (datum - Ar) /  System.Math.Abs(Br - Ar) ) ) );
                        //color_g_val = (int)(C * ( 1 -  ( (datum - Ag) / ( System.Math.Abs(Bg - Ag) ) ) ) + ( D * ( (datum - Ag) /  System.Math.Abs(Bg - Ag) ) ) );
                        //color_b_val = (int)(C * ( 1 -  ( (datum - Ab) / ( System.Math.Abs(Bb - Ab) ) ) ) + ( D * ( (datum - Ab) /  System.Math.Abs(Bb - Ab) ) ) );
                        //color_r_val = color_g_val = color_b_val = 196;
                        //int delta = D - C;
                        //int dataValue = Convert.ToInt32(inRow[ampzarray[branch_level, 1].color_index]);
                        //double gamma = (double)( (double)(dataValue - C) / (double)(delta) );
                        //// Interpolate a color based on the color palette picked
                        //color_r_val = (int)( (loColr.R + (hiColr.R - loColr.R)) * (gamma));
                        //color_g_val = (int)( (loColr.G + (hiColr.G - loColr.G)) * (gamma));
                        //color_b_val = (int)( (loColr.B + (hiColr.B - loColr.B)) * (gamma));
                        //color_a_val = 220;  
                        if (color_a_val < 0) color_a_val = Math.Abs(color_a_val);
                        if (color_r_val < 0) color_r_val = Math.Abs(color_r_val);
                        if (color_g_val < 0) color_g_val = Math.Abs(color_g_val);
                        if (color_b_val < 0) color_b_val = Math.Abs(color_b_val);
                        if (color_a_val >= 255) color_a_val = Convert.ToInt16(Math.Min(color_a_val, 255M));
                        if (color_r_val >= 255) color_r_val = Convert.ToInt16(Math.Min(color_r_val, 255M));
                        if (color_g_val >= 255) color_g_val = Convert.ToInt16(Math.Min(color_g_val, 255M));
                        if (color_b_val >= 255) color_b_val = Convert.ToInt16(Math.Min(color_b_val, 255M));
                    }
                }
                if ((ampzarray[branch_level, 1].translate_rate_x != null)   && (ampzarray[branch_level, 1].translate_rate_x != "")) translate_rate_x_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].translate_rate_x]);
                if ((ampzarray[branch_level, 1].translate_rate_y != null)   && (ampzarray[branch_level, 1].translate_rate_y != "")) translate_rate_y_val = Convert.ToDouble(thisRow[ampzarray[branch_level, 1].translate_rate_y]);

                if ((ampzarray[branch_level, 1].channel != null) && (ampzarray[branch_level, 1].channel != "") )
                {
                    // root glyphs are tied to channels 1, 2, 3 for trailing effect
                    string cid = ampzarray[branch_level ,  1].channel;
                    ch_input_id = Convert.ToInt16( cid );
                }
                //
                // Check if manual L0 Coloring override is checked
                if ( cbSetL0Color.Checked == true )
                {
                    color_r_val = nudSetL0ColorR.Value;
                    color_g_val = nudSetL0ColorG.Value; 
                    color_b_val = nudSetL0ColorB.Value;
                    color_a_val = nudSetL0ColorA.Value;
                }

                // 
                // Check is manual scaling overide is checked
                if ( cbScaleData.Checked == true )  // scale that data manually
                {
                    scale_x_val = diScaleX.Value;
                    scale_y_val = diScaleY.Value;
                    scale_z_val = diScaleZ.Value;
                    // refactor logic later
                    color_a_val = nudScaleAlpha.Value;
                }
                //Console.WriteLine(antzline1(data: rownum, id: rownum, parent_id: parent_id, translate_rate_x: translate_rate_x_val, translate_rate_y: translate_rate_y_val,
                //                            translate_x: translate_x_val, translate_y: translate_y_val, translate_z: translate_z_val, ratio: ratio_val,
                //                            scale_x: scale_x_val, scale_y: scale_y_val, scale_z: scale_z_val, geometry: geom_val, topo: topo_val,
                //                            color_a: color_a_val, color_r: color_r_val, color_g: color_g_val, color_b: color_b_val, child_count: 1));
                sw.WriteLine(antzline1(data: rownum, id: rownum++, parent_id: parent_id, translate_rate_x: translate_rate_x_val, translate_rate_y: translate_rate_y_val,
                                            ch_input_id: ch_input_id, translate_x: translate_x_val, translate_y: translate_y_val, translate_z: translate_z_val, ratio: ratio_val,
                                            scale_x: scale_x_val, scale_y: scale_y_val, scale_z: scale_z_val, geometry: geom_val, topo: topo_val,
                                            color_a: color_a_val, color_r: color_r_val, color_g: color_g_val, color_b: color_b_val, child_count: glyphsPerLevel[branch_level + 1]));
                //
                // Calculate the sine waves for pulsating visualizations

                ch4 = (double)Math.Sin(diCh4_F.Value * deg2rad * rownum) + 1;  // low 1
                ch5 = (double)Math.Sin(diCh5_F.Value * deg2rad * rownum) + 1;  // low 2
                ch6 = (double)Math.Sin(diCh6_F.Value * deg2rad * rownum) + 1;  // low 3
                ch7 = (double)Math.Sin(diCh7_F.Value * deg2rad * rownum) + 1;  // low 4
                ch8 = (double)Math.Sin(diCh8_F.Value * deg2rad * rownum) + 1;  // low 5

                
                // write channel values out
                swchan.WriteLine(chanline1(id: rownum - 5, translate_x: translate_x_val, translate_y: translate_y_val, translate_z: translate_z_val, ch4: ch4, ch5: ch5, ch6:ch6, ch7: ch7, ch8: ch8 ));
            }   // if root glyph 
            else
            {   //  Child Glyph
                // All 15/16 user selectable data are mapped onto glyph params here
                if ((ampzarray[branch_level, kid_number].translate_x != null)        && (ampzarray[branch_level, kid_number].translate_x != ""))      translate_x_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].translate_x]);
                if ((ampzarray[branch_level, kid_number].translate_y != null)        && (ampzarray[branch_level, kid_number].translate_y != ""))      translate_y_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].translate_y]);
                if ((ampzarray[branch_level, kid_number].translate_z != null)        && (ampzarray[branch_level, kid_number].translate_z != ""))      translate_z_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].translate_z]);
                if ((ampzarray[branch_level, kid_number].ratio != null)              && (ampzarray[branch_level, kid_number].ratio != ""))            ratio_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].ratio]);
                if ((ampzarray[branch_level, kid_number].scale_x != null) && (ampzarray[branch_level, kid_number].scale_x != ""))
                {
                    double scale_x_Min = Program.colData[ampzarray[branch_level, kid_number].scale_x].colMinout;
                    double scale_x_Max = Program.colData[ampzarray[branch_level, kid_number].scale_x].colMaxout;
                    // simple interpolation
                    scale_x_val = (Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].scale_x]) - scale_x_Min) / (double)(scale_x_Max - scale_x_Min);
                }
                if ((ampzarray[branch_level, kid_number].scale_y != null) && (ampzarray[branch_level, kid_number].scale_y != ""))
                {
                    double scale_y_Min = Program.colData[ampzarray[branch_level, kid_number].scale_y].colMinout;
                    double scale_y_Max = Program.colData[ampzarray[branch_level, kid_number].scale_y].colMaxout;
                    // simple interpolation
                    scale_y_val = (Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].scale_y]) - scale_y_Min) / (double)(scale_y_Max - scale_y_Min);
                }
                if ((ampzarray[branch_level, kid_number].scale_z != null) && (ampzarray[branch_level, kid_number].scale_z != ""))
                {
                    double scale_z_Min = Program.colData[ampzarray[branch_level, kid_number].scale_z].colMinout;
                    double scale_z_Max = Program.colData[ampzarray[branch_level, kid_number].scale_z].colMaxout;
                    // simple interpolation
                    scale_z_val = (Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].scale_z]) - scale_z_Min) / (double)(scale_z_Max - scale_z_Min);


                    //scale_z_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].scale_z]);
                    //double scale_z_Min = (Program.colData[ampzarray[branch_level, kid_number].scale_z].colMin != Program.colData[ampzarray[branch_level, kid_number].scale_z].colMinout)
                    //     ? Program.colData[ampzarray[branch_level, kid_number].scale_z].colMinout
                    //     : Program.colData[ampzarray[branch_level, kid_number].scale_z].colMin;
                    //double scale_z_Max = (Program.colData[ampzarray[branch_level, kid_number].scale_z].colMax != Program.colData[ampzarray[branch_level, kid_number].scale_z].colMaxout)
                    // ? Program.colData[ampzarray[branch_level, kid_number].scale_z].colMaxout
                    // : Program.colData[ampzarray[branch_level, kid_number].scale_z].colMax;


                    //// simple interpolation
                    //scale_z_val = (Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].scale_z]) - scale_z_Min) / (double)(scale_z_Max - scale_z_Min);

                }
                if ((ampzarray[branch_level, kid_number].geometry != null)           && (ampzarray[branch_level, kid_number].geometry != ""))
                {
                    switch (ampzarray[branch_level, kid_number].geometry)
                    {
                        case "Toroid":
                            geom_val = 7;
                            break;
                        case "Toroid Wire":
                            geom_val = 6;
                            break;
                        case "Cube":
                            geom_val = 1;
                            break;
                        case "Cube Wire":
                            geom_val = 0;
                            break;
                        case "Sphere":
                            geom_val = 3;
                            break;
                        case "Sphere Wire":
                            geom_val = 2;
                            break;
                        case "Cone":
                            geom_val = 5;
                            break;
                        case "Cone Wire":
                            geom_val = 4;
                            break;
                        case "Dodecahedron":
                            geom_val = 9;
                            break;
                        case "Dodecahedron Wire":
                            geom_val = 8;
                            break;
                        case "Octahedron":
                            geom_val = 11;
                            break;
                        case "Octahedron Wire":
                            geom_val = 10;
                            break;
                        case "Tetrahedron":
                            geom_val = 13;
                            break;
                        case "Tetrahedron Wire":
                            geom_val = 12;
                            break;
                        case "Icosahedron":
                            geom_val = 15;
                            break;
                        case "Icosahedron Wire":
                            geom_val = 14;
                            break;
                        case "Pin":
                            geom_val = 16;
                            break;
                        case "Pin Wire":
                            geom_val = 17;
                            break;
                        case "Cylinder":
                            geom_val = 19;
                            break;
                        case "Cylinder Wire":
                            geom_val = 18;
                            break;
                        case "Teapot":
                            geom_val = 20;
                            break;
                        case "Teapot Wire":
                            geom_val = 21;
                            break;
                        default:
                            geom_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].geometry]); 
                            break;
                    }

                }
                if ((ampzarray[branch_level, kid_number].topology != null) && (ampzarray[branch_level, kid_number].topology != ""))
                {
                    switch (ampzarray[branch_level, kid_number].topology)
                    {
                        case "Cube":
                            topo_val = 1;
                            break;
                        case "Sphere":
                            topo_val = 2;
                            break;
                        case "Torus":
                            topo_val = 3;
                            break;
                        case "Cylinder":
                            topo_val = 4;
                            break;
                        case "Pin":
                            topo_val = 5;
                            break;
                        case "Rod":
                            topo_val = 6;
                            break;
                        case "Point":
                            topo_val = 7;
                            break;
                        default:
                            topo_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].topology]);
                            break;
                    }
                }
                if ((ampzarray[branch_level, kid_number].color_a != null)            && (ampzarray[branch_level, kid_number].color_a != ""))          color_a_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].color_a]);
                if ((ampzarray[branch_level, kid_number].color_r != null)            && (ampzarray[branch_level, kid_number].color_r != ""))          color_r_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].color_r]);
                if ((ampzarray[branch_level, kid_number].color_g != null)            && (ampzarray[branch_level, kid_number].color_g != ""))          color_g_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].color_g]);
                if ((ampzarray[branch_level, kid_number].color_b != null)            && (ampzarray[branch_level, kid_number].color_b != ""))          color_b_val = Convert.ToInt16(thisRow[ampzarray[branch_level, kid_number].color_b]);
                if ((ampzarray[branch_level, kid_number].color_index != null)        && (ampzarray[branch_level, kid_number].color_index != ""))
                {  //  Special color Palette handling
                    if (Program.DP_list_items.ContainsKey(ampzarray[branch_level, kid_number].color_palette))
                    { // keep functionality to read in hexidecimal ARGB formatted numbers
                        string hexString = thisRow[ampzarray[branch_level, kid_number].color_index];
                        if (System.Text.RegularExpressions.Regex.IsMatch(hexString, @"[#]([0-9]|[a-f]|[A-F]){8}\b"))
                        { // found #AARRGGBB
                            color_a_val = int.Parse(hexString.Substring(1, 2), NumberStyles.HexNumber);
                            color_r_val = int.Parse(hexString.Substring(3, 2), NumberStyles.HexNumber);
                            color_g_val = int.Parse(hexString.Substring(5, 2), NumberStyles.HexNumber);
                            color_b_val = int.Parse(hexString.Substring(7, 2), NumberStyles.HexNumber);
                        }
                        else if (System.Text.RegularExpressions.Regex.IsMatch(hexString, @"[#]([0-9]|[a-f]|[A-F]){6}\b"))
                        {  // found #RRGGBB
                            color_a_val = int.Parse(hexString.Substring(1, 2), NumberStyles.HexNumber);
                            color_r_val = int.Parse(hexString.Substring(3, 2), NumberStyles.HexNumber);
                            color_g_val = int.Parse(hexString.Substring(5, 2), NumberStyles.HexNumber);
                            color_b_val = int.Parse(hexString.Substring(7, 2), NumberStyles.HexNumber);
                        }
                        // not a haxidecimal number
                        // get palette lo & hi number and color then linear interpolate rgb
                        double datum = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].color_index]);
                        Color loColr = Program.PaletteList[ampzarray[branch_level, kid_number].color_palette].lowColor;
                        Color hiColr = Program.PaletteList[ampzarray[branch_level, kid_number].color_palette].hiColor;
                        Int32 Ar = loColr.R; Int32 Br = hiColr.R;
                        Int32 Ag = loColr.G; Int32 Bg = hiColr.G;
                        Int32 Ab = loColr.B; Int32 Bb = hiColr.B;
                        // double C = Program.PaletteList[ampzarray[branch_level, 1].color_palette].lowNumber;  // 0; // 
                        // double D = Program.PaletteList[ampzarray[branch_level, 1].color_palette].hiNumber;  //255; // 
                        double C = (datum <    0.0) ?   0.0 :  Program.colData[ampzarray[branch_level, kid_number].color_index].colMinout;
                        double D = (datum >  255.0) ? 255.0 :  Program.colData[ampzarray[branch_level, kid_number].color_index].colMaxout;

                        E = (Program.colData[ampzarray[branch_level, kid_number].color_index].colMin != Program.colData[ampzarray[branch_level, kid_number].color_index].colMinout) 
                         ?   Program.colData[ampzarray[branch_level, kid_number].color_index].colMinout
                         :   Program.colData[ampzarray[branch_level, kid_number].color_index].colMin;
                        F = (Program.colData[ampzarray[branch_level, kid_number].color_index].colMax  != Program.colData[ampzarray[branch_level, kid_number].color_index].colMaxout)
                         ?   Program.colData[ampzarray[branch_level, kid_number].color_index].colMaxout
                         :   Program.colData[ampzarray[branch_level, kid_number].color_index].colMax;
                        

                        // simple interpolation
                        double p = (datum - C) / (double)(D - C);
                        // double p = (F - E) != 0 ? (datum - E) / (double)(F - E) : (datum - E);
                        Int32 rval = interpolate(Ar, Br, p);
                        color_r_val = rval;
                        color_g_val = interpolate(Ag, Bg, p);
                        color_b_val = interpolate(Ab, Bb, p);
                    }
                }

                if ((ampzarray[branch_level, kid_number].translate_rate_x != null)   && (ampzarray[branch_level, kid_number].translate_rate_x != "")) translate_rate_x_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].translate_rate_x]);
                if ((ampzarray[branch_level, kid_number].translate_rate_y != null)   && (ampzarray[branch_level, kid_number].translate_rate_y != "")) translate_rate_y_val = Convert.ToDouble(thisRow[ampzarray[branch_level, kid_number].translate_rate_y]);
                // evenly space out rotations
                // Refactor next line
                double glyph_rotation_deg = ((glyphsPerLevel[branch_level] > 1) ? ((360) / glyphsPerLevel[branch_level]) * kid_number : 0);
                    
                // Now deal with channels
                if ((pastRow.Count > 0) && (ampzarray[branch_level, kid_number].channel != null) && (ampzarray[branch_level, kid_number].channel != ""))
                {
                    if (Convert.ToDouble(thisRow[ampzarray[0, 1].translate_x]) > Convert.ToDouble(pastRow[ampzarray[0, 1].translate_x]))
                    {
                        ch_input_id = 4;
                    }
                    else ch_input_id = 6;
                    //string cid = ampzarray[branch_level, kid_number].channel;
                    //ch_input_id = Convert.ToInt16( cid );
                }
                
                
                //Console.WriteLine(antzline1(data: rownum, id: rownum, parent_id: parent_id, child_count: glyphsPerLevel[branch_level - 1], branch_level: branch_level,
                //             translate_rate_x: translate_rate_x_val, translate_rate_y: translate_rate_y_val,
                //             translate_x: glyph_rotation_deg, translate_y: translate_y_val, translate_z: translate_z_val, ratio: ratio_val,
                //             scale_x: scale_x_val, scale_y: scale_y_val, scale_z: scale_z_val, geometry: geom_val, topo: topo_val,
                //             color_a: color_a_val, color_r: color_r_val, color_g: color_g_val, color_b: color_b_val));
                sw.WriteLine(antzline1(data: rownum, id: rownum++, parent_id: parent_id, child_count: glyphsPerLevel[branch_level+1], branch_level: branch_level,
                             ch_input_id: ch_input_id, translate_rate_x: translate_rate_x_val, translate_rate_y: translate_rate_y_val,
                             translate_x: glyph_rotation_deg, translate_y: translate_y_val, translate_z: translate_z_val, ratio: ratio_val,
                             scale_x: scale_x_val, scale_y: scale_y_val, scale_z: scale_z_val, geometry: geom_val, topo: topo_val,
                             color_a: color_a_val, color_r: color_r_val, color_g: color_g_val, color_b: color_b_val));
                // write channel values out
                //swchan.WriteLine(chanline1(id: rownum - 5, translate_x: translate_x_val, translate_y: translate_y_val, translate_z: translate_z_val));
            }
            //child_count = glyphsPerLevel[branch_level+1];
            child_count = (branch_level  <= MAXD) ? glyphsPerLevel[branch_level+1] : 0;
            
            if ((branch_level < MAXD)   )
            {
                for (int kids = 1; kids <= child_count; kids++)
                {
                    doGlyph(current_row, branch_level + 1 , kids, thisRow, pastRow );
                }
                branch_level++;
            }
        }  // doGlyph

        Int32 interpolate(Int32 a, Int32 b, double p)
        {
            return (Int32)(a * (1 - p) + b * p);
        }

        public string antzline1(int id = 7, int type = 5, int data = 0, int selected = 0, int parent_id = 0, int branch_level = 0, int child_id = 0, int child_index = 0, int child_count = 0, int ch_input_id = 0, int ch_output_id = 0, int ch_last_updated = 0, int average = 0, int sample = 1, int aux_a_x = 0, int aux_a_y = 0, int aux_a_z = 0, int aux_b_x = 0, int aux_b_y = 0, int aux_b_z = 0, int color_shift = 0, double rotate_vec_x = 0, double rotate_vec_y = 0, double rotate_vec_z = 0, double rotate_vec_s = 1, double scale_x = 1, double scale_y = 1, double scale_z = 1, double translate_x = 0.0, double translate_y = 0.0, double translate_z = 0.0, double tag_offset_x = 0, double tag_offset_y = 0, double tag_offset_z = 5.5, double rotate_rate_x = 0, double rotate_rate_y = 0, double rotate_rate_z = 0, double rotate_x = 0, double rotate_y = 0, double rotate_z = 0, double scale_rate_x = 0, double scale_rate_y = 0, double scale_rate_z = 0, double translate_rate_x = 0, double translate_rate_y = 0, double translate_rate_z = 0, double translate_vec_x = 0, double translate_vec_y = 0, double translate_vec_z = 0, int shader = 0, int geometry = 7, int line_width = 1, int point_size = 0, double ratio = 0.1, int color_index = 0, int color_r = 50, int color_g = 101, int color_b = 101, int color_a = 255, int color_fade = 0, int texture_id = 0, int hide = 0, int freeze = 0, int topo = 3, int facet = 0, int auto_zoom_x = 0, int auto_zoom_y = 0, int auto_zoom_z = 0, int trigger_hi_x = 0, int trigger_hi_y = 0, int trigger_hi_z = 0, int trigger_lo_x = 0, int trigger_lo_y = 0, int trigger_lo_z = 1, double set_hi_x = 0, double set_hi_y = 0, double set_hi_z = 0, double set_lo_x = 0, double set_lo_y = 0, double set_lo_z = 0, double proximity_x = 0, double proximity_y = 0, double proximity_z = 0, int proximity_mode_x = 0, int proximity_mode_y = 0, int proximity_mode_z = 0, int segments_x = 16, int segments_y = 16, int segments_z = 0, int tag_mode = 0, int format_id = 0, int table_id = 0, int record_id = 0, int size = 420)
        {
            return (id.ToString() + "," + type.ToString() + "," + data.ToString() + "," + selected.ToString() + "," + parent_id.ToString() + "," + branch_level.ToString() + "," + child_id.ToString() + "," + child_index.ToString() + "," + child_count.ToString() + "," + ch_input_id.ToString() + "," + ch_output_id.ToString() + "," + ch_last_updated.ToString() + "," + average.ToString() + "," + sample.ToString() + "," + aux_a_x.ToString() + "," + aux_a_y.ToString() + "," + aux_a_z.ToString() + "," + aux_b_x.ToString() + "," + aux_b_y.ToString() + "," + aux_b_z.ToString() + "," + color_shift.ToString() + "," + rotate_vec_x.ToString() + "," + rotate_vec_y.ToString() + "," + rotate_vec_z.ToString() + "," + rotate_vec_s.ToString() + "," + scale_x.ToString() + "," + scale_y.ToString() + "," + scale_z.ToString() + "," + translate_x.ToString() + "," + translate_y.ToString() + "," + translate_z.ToString() + "," + tag_offset_x.ToString() + "," + tag_offset_y.ToString() + "," + tag_offset_z.ToString() + "," + rotate_rate_x.ToString() + "," + rotate_rate_y.ToString() + "," + rotate_rate_z.ToString() + "," + rotate_x.ToString() + "," + rotate_y.ToString() + "," + rotate_z.ToString() + "," + scale_rate_x.ToString() + "," + scale_rate_y.ToString() + "," + scale_rate_z.ToString() + "," + translate_rate_x.ToString() + "," + translate_rate_y.ToString() + "," + translate_rate_z.ToString() + "," + translate_vec_x.ToString() + "," + translate_vec_y.ToString() + "," + translate_vec_z.ToString() + "," + shader.ToString() + "," + geometry.ToString() + "," + line_width.ToString() + "," + point_size.ToString() + "," + ratio.ToString() + "," + color_index.ToString() + "," + color_r.ToString() + "," + color_g.ToString() + "," + color_b.ToString() + "," + color_a.ToString() + "," + color_fade.ToString() + "," + texture_id.ToString() + "," + hide.ToString() + "," + freeze.ToString() + "," + topo.ToString() + "," + facet.ToString() + "," + auto_zoom_x.ToString() + "," + auto_zoom_y.ToString() + "," + auto_zoom_z.ToString() + "," + trigger_hi_x.ToString() + "," + trigger_hi_y.ToString() + "," + trigger_hi_z.ToString() + "," + trigger_lo_x.ToString() + "," + trigger_lo_y.ToString() + "," + trigger_lo_z.ToString() + "," + set_hi_x.ToString() + "," + set_hi_y.ToString() + "," + set_hi_z.ToString() + "," + set_lo_x.ToString() + "," + set_lo_y.ToString() + "," + set_lo_z.ToString() + "," + proximity_x.ToString() + "," + proximity_y.ToString() + "," + proximity_z.ToString() + "," + proximity_mode_x.ToString() + "," + proximity_mode_y.ToString() + "," + proximity_mode_z.ToString() + "," + segments_x.ToString() + "," + segments_y.ToString() + "," + segments_z.ToString() + "," + tag_mode.ToString() + "," + format_id.ToString() + "," + table_id.ToString() + "," + record_id.ToString() + "," + size.ToString());
        }
        
        public string chanline1(int id = 1,  double translate_x = 0.0, double translate_y = 0.0, double translate_z = 0.0 , double ch4 = 0.0, double ch5 = 0.0 ,double ch6 = 0.0, double ch7 = 0.0, double ch8 = 0.0 )
        {
            return (id.ToString() + "," + translate_x.ToString() + "," + translate_y.ToString() + "," + translate_z.ToString() + "," + ch4.ToString() + "," + ch5.ToString() + "," + ch6.ToString() + "," + ch7.ToString() + "," + ch8.ToString());
        }
        
        public string antzline2(int id = 24, int type = 5, int data = 24, int selected = 0, int parent_id = 0, int branch_level = 1, int child_id = 0, int child_index = 0, int child_count = 1, int ch_input_id = 0, int ch_output_id = 0, int ch_last_updated = 0, int average = 0, int sample = 1, int aux_a_x = 0, int aux_a_y = 0, int aux_a_z = 0, int aux_b_x = 0, int aux_b_y = 0, int aux_b_z = 0, int color_shift = 0, double rotate_vec_x = 0, double rotate_vec_y = 0, double rotate_vec_z = 0, double rotate_vec_s = 1, double scale_x = 1, double scale_y = 1, double scale_z = 1, double translate_x = 0.0, double translate_y = 0.0, double translate_z = 0.0, double tag_offset_x = 0, double tag_offset_y = 0, double tag_offset_z = 5.5, double rotate_rate_x = 0, double rotate_rate_y = 0, double rotate_rate_z = 0, double rotate_x = 0, double rotate_y = 0, double rotate_z = 0, double scale_rate_x = 0, double scale_rate_y = 0, double scale_rate_z = 0, double translate_rate_x = 0, double translate_rate_y = 0, double translate_rate_z = 0, double translate_vec_x = 0, double translate_vec_y = 0, double translate_vec_z = 0, int shader = 0, int geometry = 16, int line_width = 1, int point_size = 0, double ratio = 0.1, int color_index = 0, int color_r = 50, int color_g = 101, int color_b = 101, int color_a = 255, int color_fade = 0, int texture_id = 0, int hide = 0, int freeze = 0, int topo = 5, int facet = 0, int auto_zoom_x = 0, int auto_zoom_y = 0, int auto_zoom_z = 0, int trigger_hi_x = 0, int trigger_hi_y = 0, int trigger_hi_z = 0, int trigger_lo_x = 0, int trigger_lo_y = 0, int trigger_lo_z = 1, double set_hi_x = 0, double set_hi_y = 0, double set_hi_z = 0, double set_lo_x = 0, double set_lo_y = 0, double set_lo_z = 0, double proximity_x = 0, double proximity_y = 0, double proximity_z = 0, int proximity_mode_x = 0, int proximity_mode_y = 0, int proximity_mode_z = 0, int segments_x = 16, int segments_y = 16, int segments_z = 0, int tag_mode = 0, int format_id = 0, int table_id = 0, int record_id = 0, int size = 420)
        {
            return ("id=" + id.ToString() + ",type=" + type.ToString() + ",data=" + data.ToString() + ",selected=" + selected.ToString() + ",parent_id=" + parent_id.ToString() + ",branch_level=" + branch_level.ToString() + ",child_id=" + child_id.ToString() + ",child_index=" + child_index.ToString() + ",child_count=" + child_count.ToString() + ",scale_x=" + scale_x.ToString() + ",scale_y=" + scale_y.ToString() + scale_z + scale_z.ToString() + ",translate_x= >>>> " + translate_x.ToString() + " < ,translate_y=" + translate_y.ToString() + ",translate_z=" + translate_z.ToString() + ",=" + tag_offset_x.ToString() + ",=" + tag_offset_y.ToString() + ",=" + tag_offset_z.ToString() + ",=" + rotate_rate_x.ToString() + ",=" + rotate_rate_y.ToString() + ",=" + rotate_rate_z.ToString() + ",=" + rotate_x.ToString() + ",=" + rotate_y.ToString() + ",=" + rotate_z.ToString() + ",=" + scale_rate_x.ToString() + ",=" + scale_rate_y.ToString() + ",=" + scale_rate_z.ToString() + ",=" + translate_rate_x.ToString() + ",=" + translate_rate_y.ToString() + ",=" + translate_rate_z.ToString() + ",=" + translate_vec_x.ToString() + ",=" + translate_vec_y.ToString() + ",=" + translate_vec_z.ToString() + ",=" + shader.ToString() + ",=" + geometry.ToString() + ",=" + line_width.ToString() + ",=" + point_size.ToString() + ",=" + ratio.ToString() + ",=" + color_index.ToString() + ",=" + color_r.ToString() + ",=" + color_g.ToString() + ",=" + color_b.ToString() + ",=" + color_a.ToString() + ",=" + color_fade.ToString() + ",=" + texture_id.ToString() + ",=" + hide.ToString() + ",=" + freeze.ToString() + ",=" + topo.ToString() + ",=" + facet.ToString() + ",=" + auto_zoom_x.ToString() + ",=" + auto_zoom_y.ToString() + ",=" + auto_zoom_z.ToString() + ",=" + trigger_hi_x.ToString() + ",=" + trigger_hi_y.ToString() + ",=" + trigger_hi_z.ToString() + ",=" + trigger_lo_x.ToString() + ",=" + trigger_lo_y.ToString() + ",=" + trigger_lo_z.ToString() + ",=" + set_hi_x.ToString() + ",=" + set_hi_y.ToString() + ",=" + set_hi_z.ToString() + ",=" + set_lo_x.ToString() + ",=" + set_lo_y.ToString() + ",=" + set_lo_z.ToString() + ",=" + proximity_x.ToString() + ",=" + proximity_y.ToString() + ",=" + proximity_z.ToString() + ",=" + proximity_mode_x.ToString() + ",=" + proximity_mode_y.ToString() + ",=" + proximity_mode_z.ToString() + ",=" + segments_x.ToString() + ",=" + segments_y.ToString() + ",=" + segments_z.ToString() + ",=" + tag_mode.ToString() + ",=" + format_id.ToString() + ",=" + table_id.ToString() + ",=" + record_id.ToString() + ",=" + size.ToString());
        }

        public string doHeader()
        {
            return (
                    "id,type,data,selected,parent_id,branch_level,child_id,child_index,child_count,ch_input_id,ch_output_id,ch_last_updated,average,sample,aux_a_x,aux_a_y,aux_a_z,aux_b_x,aux_b_y,aux_b_z,color_shift,rotate_vec_x,rotate_vec_y,rotate_vec_z,rotate_vec_s,scale_x,scale_y,scale_z,translate_x,translate_y,translate_z,tag_offset_x,tag_offset_y,tag_offset_z,rotate_rate_x,rotate_rate_y,rotate_rate_z,rotate_x,rotate_y,rotate_z,scale_rate_x,scale_rate_y,scale_rate_z,translate_rate_x,translate_rate_y,translate_rate_z,translate_vec_x,translate_vec_y,translate_vec_z,shader,geometry,line_width,point_size,ratio,color_index,color_r,color_g,color_b,color_a,color_fade,texture_id,hide,freeze,topo,facet,auto_zoom_x,auto_zoom_y,auto_zoom_z,trigger_hi_x,trigger_hi_y,trigger_hi_z,trigger_lo_x,trigger_lo_y,trigger_lo_z,set_hi_x,set_hi_y,set_hi_z,set_lo_x,set_lo_y,set_lo_z,proximity_x,proximity_y,proximity_z,proximity_mode_x,proximity_mode_y,proximity_mode_z,segments_x,segments_y,segments_z,tag_mode,format_id,table_id,record_id,size\n" +
                    "1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,0.00,0.00,1.00,1.00,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,16,16,0,0,0,0,0,420\n" +
                    "2,1,2,0,0,0,0,2,3,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,0.482963,0.836516,-0.258819,1.00,1.00,1.00,-27.00,-25.00,9.00,0.00,0.00,0.00,0.00,0.00,0.00,75.00,30.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,16,16,0,0,0,0,0,420\n" +
                    "3,1,3,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,0.00,0.00,-1.00,1.00,1.00,1.00,-0.500000,0.00,571.750000,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,16,16,0,0,0,0,0,420\n" +
                    "4,1,4,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,0.00,1.00,-0.00,1.00,1.00,1.00,0.00,-90.00,7.00,0.00,0.00,0.00,0.00,0.00,0.00,90.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,16,16,0,0,0,0,0,420\n" +
                    "5,1,5,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,-1.00,0.00,-0.00,1.00,1.00,1.00,85.00,0.00,7.00,0.00,0.00,0.00,0.00,0.00,0.00,90.00,270.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,16,16,0,0,0,0,0,420\n" +
                    "6,6,6,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.00,0.00,0.00,0.00,1.00,1.00,1.00,1.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,1.00,0.00,0.100000,3,0,0,255,150,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,0,0,0,12,6,0,0,0,0,0,420"
                    );
        }  //  doHeader
        
        public void colStat_ListChanged(object sender, ListChangedEventArgs e)
        {  // Refactor this
            //MessageBox.Show("List changed =" + e.ToString() + " foo");
        }
        
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        { // Open file Menu item clicked...
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            String inLine;
            String myField;
            BindingList<colStats> colStat = new BindingList<colStats>();
            BindingSource colStatBS = new BindingSource();
            StreamReader fileReader = null;

            int lineNumber = 0;
            colStat.ListChanged += new ListChangedEventHandler(colStat_ListChanged);
                   
            dgv1.AutoGenerateColumns = true;
            
             
            //openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "csv files (*.csv)|*.csv";
            openFileDialog1.FilterIndex = 2;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                inFile = openFileDialog1.FileName;
                statusBox.Text = "Status: Starting to read file: " + openFileDialog1.FileName;
                try
                {
                    using ( fileReader = File.OpenText(inFile))
                    using ( csvResult = new CsvHelper.CsvReader(fileReader))
                    {
                        
                        csvResult.Read(); // this does all the Magic reading
                        inLine = String.Join(" , ", csvResult.FieldHeaders);
                        groupBox3.Text = inFile.ToString();

                        foreach (String headR in csvResult.FieldHeaders)
                        { // adds the header row to the colData object
                            Program.colData.Add(headR.ToString(), new colStats());
                            Program.colData[headR.ToString()].colName = headR.ToString();
                            // TODO check for duplicates in list_items
                            DD_list_items.Add(headR, headR);
                            DG_list_items.Add(headR, headR);
                            DT_list_items.Add(headR, headR);
                            //Program.DP_list_items.Add(headR, headR);
                        }

                        do 


                        // this reads the file and populates the Program.colData object
                        {

                            foreach (String rowR in csvResult.FieldHeaders)
                            {
                                myField = csvResult.GetField(rowR.ToString());
                                if (lineNumber == 0)
                                {
                                    Program.colData[rowR].colMax = Convert.ToDouble(myField);
                                    Program.colData[rowR].colMin = Convert.ToDouble(myField);
                                }
                                if (!System.Text.RegularExpressions.Regex.IsMatch(myField, @"[#]([0-9]|[a-f]|[A-F]){8}\b"))
                                {
                                    if (Convert.ToDouble(Program.colData[rowR].colMax) < Convert.ToDouble(myField)) Program.colData[rowR].colMax = Convert.ToDouble(myField);
                                    if (Convert.ToDouble(Program.colData[rowR].colMin) >= Convert.ToDouble(myField)) Program.colData[rowR].colMin = Convert.ToDouble(myField);
                                    Program.colData[rowR].colNum++;
                                    Program.colData[rowR].colAvg += Convert.ToDouble(myField);
                                    Program.colData[rowR].colMinout = Program.colData[rowR].colMin;
                                    Program.colData[rowR].colMaxout = Program.colData[rowR].colMax;
                                }
                                else
                                {
                                    Program.colData[rowR].colNum++;

                                }

                            }
                            lineNumber++;
                        } while (csvResult.Read());
                        
                        foreach (String rowR in csvResult.FieldHeaders)
                        {
                            DataGridViewTextBoxColumn myColumn = new DataGridViewTextBoxColumn();
                            myColumn.DataPropertyName = rowR;
                            myColumn.HeaderText = rowR;
                            colStat.Add(Program.colData[rowR]);
                            dropBoxItems.Add(rowR.ToString(), rowR.ToString());
                        }
                        
                        dSet.Tables.Add("csv1");
                        List<String> rowName    = new List<string>();    // 0
                        List<String> rowMin     = new List<string>();    // 1
                        List<String> rowMax     = new List<string>();    // 2
                        List<String> rowAvg     = new List<string>();    // 3
                        List<String> rowNum     = new List<string>();    // 4
                        List<String> rowMinout  = new List<string>();    // 5
                        List<String> rowMaxout = new List<string>();     // 6
                        List<String> rowClipMinout = new List<string>(); // 7
                        List<String> rowClipMaxout = new List<string>(); // 8
                        DataColumn columnstat   = new DataColumn();
                        columnstat.ColumnName   = "Stats";
                        dSet.Tables["csv1"].Columns.Add(columnstat);
                        rowName.Add("rowName");
                        rowMin.Add("Min");
                        rowMax.Add("Max");
                        rowAvg.Add("Avg");
                        rowNum.Add("Num");
                        rowMinout.Add("Min Out");
                        rowMaxout.Add("Max Out");
                        rowClipMinout.Add("Clip Min Out");                        
                        rowClipMaxout.Add("Clip Max Out");

                        foreach (String rowR in csvResult.FieldHeaders)
                        {

                            DataColumn column = new DataColumn();
                            column.ColumnName = Program.colData[rowR].colName;
                            dSet.Tables["csv1"].Columns.Add(column);
                            rowName.Add(Program.colData[rowR].colName);
                            rowMin.Add(Program.colData[rowR].colMin.ToString());
                            rowMax.Add(Program.colData[rowR].colMax.ToString());
                            rowAvg.Add((Program.colData[rowR].colAvg / Program.colData[rowR].colNum).ToString());
                            rowNum.Add(Program.colData[rowR].colNum.ToString());
                            rowMinout.Add(Program.colData[rowR].colMin.ToString());
                            rowMaxout.Add(Program.colData[rowR].colMax.ToString());
                            rowClipMinout.Add(Program.colData[rowR].colMin.ToString());
                            rowClipMaxout.Add(Program.colData[rowR].colMax.ToString());

                        }
                        dSet.Tables["csv1"].Rows.Add(rowMin.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowMax.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowAvg.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowNum.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowMinout.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowMaxout.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowClipMinout.ToArray());
                        dSet.Tables["csv1"].Rows.Add(rowClipMaxout.ToArray());

                        
                        dgv1.DataSource = dSet.Tables["csv1"];
                    }
                    colStatBS.DataSource = dSet.Tables["csv1"];
                    foreach (DataGridViewColumn col in dgv1.Columns)
                        col.SortMode = DataGridViewColumnSortMode.NotSortable;
                    statusBox.Text = "Status: CSV file successfully read. ";
                    Program.wfMainStep1 = true;
                    wfMainStep1.Value = 100;
                    csv_FileOpened = true;
                    saveToolStripMenuItem.Enabled = true;
                    saveAsToolStripMenuItem.Enabled = true;
                    
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                    if (ex.Message.Contains("cannot access the file"))
                    {
                        MessageBox.Show("File 1 is open and cannot be read. \r\n\r\nPlease close file 1 and try again.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        Console.WriteLine("Exception Reading CSV File 1: {0} ...", ex.Message);
                    }
                    statusBox.Text = "Status: Problem reading file Error: " + ex.Message;
                }
                fileReader.Dispose();
            }

        }// Open file Menu item clicked...

        private void timer1_Tick(object sender, EventArgs e)
        {
            counter++;
            if (counter == 20)
            {
                timer1.Stop();
                this.Close();
            }
        }

        private void importDefaultsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AutoScale_Defaults AS_Def = new AutoScale_Defaults();
            AS_Def.ShowDialog();
        }

        private void colorPalateEditorToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Palette_Manager ColorPal = new Palette_Manager();
            ColorPal.ShowDialog();
        }

        private void dgv1_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            // Refactor
            //MessageBox.Show("List changed =" + e.ToString() + " foo");
            // update colStats

            // Reset bindings
        }

        private void gPSSimulator1ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_GPS_Sim1 fgsim1 = new Form_GPS_Sim1();
            fgsim1.Show();
        }

        private void readTCXToolStripMenuItem_Click(object sender, EventArgs e)
        {

            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            
            string outstr = "";
            List<Position> ltp = new List<Position>();
            Position ltpc = new Position();
            tcxPoint datas = new tcxPoint();
            int indx = 1;
            
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                inFile = openFileDialog1.FileName;
                FileStream outfileStream = new FileStream(openFileDialog1.FileName+ "-outfile.csv", FileMode.OpenOrCreate);
                StreamWriter swout = new StreamWriter(outfileStream);
                
                statusBox.Text = "Status: Starting to read file: " + openFileDialog1.FileName;
                outstr += "Ide,　lat, lon, AltitudeMeters, Timex, DistanceMeters, HeartRateBpm, Cadence, Speed, cal\r\n";
                swout.WriteLine(outstr);
                try
                {
                    Activity activity = GarminUtils.ConvertTCS(inFile);
                    int count = 0;
                    foreach (Lap lap in activity.Laps)
                    {
                        datas.cal = lap.Calories;
                        foreach (Track track in lap.Tracks)
                        {
                            foreach (TrackPoint tp in track.TrackPoints)
                            {
                                datas.Timex = tp.Timex;
                                datas.AltitudeMeters = tp.AltitudeMeters;
                                datas.DistanceMeters = tp.DistanceMeters;
                                datas.HeartRateBpm = tp.HeartRateBpm;
                                //foreach ( Extens exes in tp.Extras)
                                //{
                                //    datas.Cadence = exes.Cadance;
                                //    datas.Speed = exes.Speedy;
                                //}
                                datas.Cadence = (int)tp.Cadence;
                                datas.Speed = tp.Speed;
                                foreach (Position latlon in tp.Positionx)
                                {
                                    datas.lat = latlon.LatitudeDegrees;
                                    datas.lon = latlon.LongitudeDegrees;
                                } 
                                datas.name = (indx++).ToString();
                                swout.WriteLine(datas.ToString());
                            }
                        }
                    }
                    statusBox.Text = String.Format("{0} Track Points Processed.", count);
                    swout.Close();
                }

                catch (Exception ex)
                {
                    //MessageBox.Show("Error: Could not read file from disk. Original error: " + ex.Message);
                    statusBox.Text = "Status: Problem reading file Error: " + ex.Message;
                }
            }
        }

        private void saveCroppedCSVAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            statusBox.Text = "Status: now Saving the Antz File...";
            saveFileDialog1.ShowDialog();
            var file = new FileInfo(saveFileDialog1.FileName);
            int id;
            bool skip_this_data_point = false;
            Dictionary<string, string> thisRow = new Dictionary<string, string>();
            Dictionary<string, string> pastRow = new Dictionary<string, string>();
            string saveFileName = saveFileDialog1.FileName;
            
            double MinoutA = -90.0;
            double MaxoutB =  90.0;
            double DataMin =  -1.0;
            double DataMax =   1.0;

            
            /*
                * Psuedo Code....
                * - select output file via SaveFile Dialogue
                * - open csv file, rewind to start
                * - read a row from the csv input file
                * - Do
                * -   if row data withing cropping region
                * -       write antz row to output file
                * - While ! end of rows
                * 
                */


            id = 0;
            rownum = 22;
            using (CsvReader csvResult = new CsvReader(new StreamReader(inFile)))
            {
                using (sw = new StreamWriter(saveFileName))
                {
                    csvResult.Read();
                    string[] headers = csvResult.FieldHeaders;
                    for (int i = 0; i < csvResult.FieldHeaders.Count(); i++)
                        {
                            sw.Write(headers[i].ToString() + " , ");
                        }
                    sw.WriteLine();
                    do
                    {   // 
                        //string[] headers = csvResult.FieldHeaders;
                        for (int i = 0; i < csvResult.FieldHeaders.Count(); i++)
                        {
                            if (!String.IsNullOrEmpty(dgv1.Rows[0].Cells[headers[i]].Value.ToString()))
                            {
                                DataMin = (double)Convert.ToDouble((dgv1.Rows[0].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[1].Cells[headers[i]].Value.ToString()))
                            {
                                DataMax = (double)Convert.ToDouble((dgv1.Rows[1].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[4].Cells[headers[i]].Value.ToString()))
                            {
                                MinoutA = (double)Convert.ToDouble((dgv1.Rows[4].Cells[headers[i]].Value.ToString()));
                            }
                            if (!String.IsNullOrEmpty(dgv1.Rows[5].Cells[headers[i]].Value.ToString()))
                            {
                                MaxoutB = (double)Convert.ToDouble((dgv1.Rows[5].Cells[headers[i]].Value.ToString()));
                            }
                            if ((DataMin != MinoutA) || (DataMax != MaxoutB))
                            {   // Autoscale Output Value
                                thisRow[headers[i]] = ((((MaxoutB - MinoutA) * (Convert.ToDouble(csvResult[i]) - DataMin)) / (DataMax - DataMin)) + MinoutA).ToString();
                            }
                            else
                            {   // No AutoScale Output Value
                                thisRow[headers[i]] = csvResult[i];
                            }
                            if ((Convert.ToDouble(csvResult[i]) < Convert.ToDouble(dgv1.Rows[6].Cells[headers[i]].Value))    // clip min out
                                 ||
                                 (Convert.ToDouble(csvResult[i]) > Convert.ToDouble(dgv1.Rows[7].Cells[headers[i]].Value)))  // Clip Max Out
                            {
                                skip_this_data_point = true;
                            }
                        }
                        if (skip_this_data_point != true)
                        {


                            if ((id % nudSkipFactor.Value) == 0)
                            {
                                // check each row to see if it is within clipMinout and clipMaxout range
                                //write_CSV_Data(thisRow);
                                for (int i = 0; i < csvResult.FieldHeaders.Count(); i++)
                                {
                                    sw.Write(thisRow[headers[i]] + " , "); // Refactor - this adds an extra , at the end ot the line
                                }
                                sw.WriteLine();
                                //csvResult.Row.
                            }
                            id++;
                        }
                        skip_this_data_point = false;
                    } while (csvResult.Read());
                    sw.Close();
                }
                }
        } //readTCXToolStripMenuItem_Click

        private void write_CSV_Data( Dictionary<string, string> thisRow)
        {

        }// write_CSV_Data

        private void smartDecimationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form_SmartDecimation form_SLERP = new Form_SmartDecimation();
            form_SLERP.Show();

        }

        private void open2AntzCSVForDiffMappingToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult dialogResult = System.Windows.Forms.DialogResult.Yes;
            if (numberOfLevelsSet == true)
            {
                dialogResult = MessageBox.Show("You have set the number of levels earlier. \r\n\tThis action will will result in the number of level reset and possible loss of previous mapping selections. \r\nDo you wish to proceed?", "Warning: Number of levels set already!", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
            }
            if (dialogResult == System.Windows.Forms.DialogResult.Yes)
            {
                // Reset the number of levels
                Program.DiffGlyphMappingWorkflow = true;
                tableLayoutPanel1.Controls.Clear();
                groupBox2.Visible = false;

                //int trow = tableLayoutPanel1.RowCount;
                //int origGLCcount = glyphLevelControls.Count;
                //if (origGLCcount > 0)
                //{
                //    for (int rj = 0; rj < origGLCcount; rj++)
                //    {
                //        glyphLevelControls.RemoveAt((origGLCcount - 1) - rj);
                //    }
                //}
                //for (int ri = trow - 1; ri == 0; ri--)
                //{
                //    tableLayoutPanel1.Controls.RemoveAt(ri);
                //}

                exTC1.ShowTabPage(tab_GlyphDiff);
                exTC1.SelectTab(tab_GlyphDiff);
                //tab_GlyphDiff.
            }
            
        }

        private void btX_AntzFile1_Click(object sender, EventArgs e)
        {
            openGD_File1.ShowDialog();
        }

        private void openGlyfDifFile1_FileOk(object sender, CancelEventArgs e)
        {
            txtBox_File1.Text = openGD_File1.FileName.ToString();
            stepItem1.Value = 100;
            btX_AntzFile2.Enabled = true;
            Program.gDiff1 = true;
            if (Program.gDiff1 &&  Program.gDiffUse1file)
            {
                txtBox_DiffGlyph.Visible = true;
                txtBox_DiffGlyph.Enabled = true;
                
                btX_DiffGlyphs.Visible = true;
                btX_DiffGlyphs.Enabled = true;
            }
            //btX_AntzFile2.Visible = true;
        }

        private void btX_AntzFile2_Click(object sender, EventArgs e)
        {
            openGD_File2.ShowDialog();
        }

        private void openGD_File2_FileOk(object sender, CancelEventArgs e)
        {
            txtBox_File2.Text = openGD_File2.FileName.ToString();
            //stepItem2.Value = 100;
            Program.gDiff2 = true;
            if (Program.gDiff1 && Program.gDiff2 && !Program.gDiffUse1file)
            {
                txtBox_DiffGlyph.Visible = true;
                txtBox_DiffGlyph.Enabled = true;
                btX_DiffGlyphs.Visible = true;
                btX_DiffGlyphs.Enabled = true;
            }
            
        }

        private void btX_DiffGlyphs_Click(object sender, EventArgs e)
        {
            bool extractResults = false;

            // DialogResult dialogResult = MessageBox.Show("Diffing the Glyphs", "Glyph Diff", MessageBoxButtons.YesNoCancel);

            extractResults = extractFirstTwoGlyphs4Diff(openGD_File1);


            if (extractResults)
            {
                wfMainStep2.Value = 99;
                stepItem3.Value = 99;
                txtBox_DiffGlyph.BackColor = System.Drawing.Color.LightGreen;
                txtBox_DiffGlyph.Text = "Files are compatible, proceeding to Mapping.";
                do_Levels_Map_Diff();
            }
            else 
            {
                stepItem3.Value = 50;
                wfMainStep2.Value = 50;
                txtBox_DiffGlyph.BackColor = System.Drawing.Color.LightSalmon;
                txtBox_DiffGlyph.Text = "Files are not compatible for Diff Glyph Mapping.";
            }
        }
        
        /// <summary>
        /// extractFirstTwoGlyphs4Diff: given two antz files termine if the are different enough to map
        /// </summary>
        /// <param name="openGD_File1"></param>
        /// 
        /// <returns>True, if ok for Glyph Diff Mapping, False if files are too dissimilar.</returns>
        private bool extractFirstTwoGlyphs4Diff(OpenFileDialog openGD_File1)
        {
            bool retValue = false;
            List<sgxAntz> nodeList1 = new List<sgxAntz>();
            List<sgxAntz> nodeList2 = new List<sgxAntz>();
            sgxAntz tmpNode = new sgxAntz();
            bool pastNode1 = false; 
            bool pastNode2 = false;
            int numberOfLevel0 = 0;
            int previousBranchLevel = 0;
            int childNumber = 1;
            int[] GpL1 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            int[] GpL2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            try {
                using (csvResult1 = new CsvHelper.CsvReader(File.OpenText(openGD_File1.FileName)))
                {
                    csvResult1.Configuration.HasHeaderRecord = true;
                    csvResult1.Read();
                    
                    if ( check_if_file_in_Antz_Format(csvResult1.FieldHeaders) ){
                        Debug.Print("Yes file is in Antz Format");
                    } 
                    else
                    {
                        throw ( new Exception("File not in Antz Format"));
                    }
                    do
	                {
                        tmpNode = parseAntzNode(csvResult1);
                        if (tmpNode.type != 5) continue;
                        if ((!pastNode1) )
                        {
                            if (tmpNode.branch_level == 0) numberOfLevel0++;
                            if (tmpNode.branch_level == previousBranchLevel)
                            {
                                tmpNode.child_id = childNumber++;
                            }
                            else
                            {
                                childNumber = 1;
                                tmpNode.child_id = childNumber++;
                            }
                            if ((tmpNode.branch_level >= 0) && (numberOfLevel0 < 2))
                            {
                                GpL1[tmpNode.branch_level]++;
                                nodeList1.Add(tmpNode);
                                previousBranchLevel = tmpNode.branch_level;
                                continue;
                            }
                            else
                            {
                                pastNode1 = true;
                                numberOfLevel0 = 0;
                            }
                        }
                        if ((pastNode1) && (!pastNode2))
                        {
                            if (tmpNode.branch_level == 0) numberOfLevel0++;
                            if (tmpNode.branch_level == previousBranchLevel)
                            {
                                tmpNode.child_id = childNumber++;
                            }
                            else
                            {
                                childNumber = 1;
                                tmpNode.child_id = childNumber++;
                            }
                            if ((tmpNode.branch_level >= 0) && (numberOfLevel0 < 2))
                            {
                                GpL2[tmpNode.branch_level]++;
                                nodeList2.Add(tmpNode);
                                previousBranchLevel = tmpNode.branch_level;
                                continue;
                            }
                            else
                            {
                                numberOfLevel0 = 0;
                                pastNode2 = true;
                            }
                        }
                       
	                } while (csvResult1.Read() && (!pastNode2) );
                }
                analyzeGlyphDiffs(nodeList1, nodeList2);
                retValue= true;
                for (int i = 0; i < 16; i++)
                {
                    glyphsPerLevel[i] = GpL1[i];
                }
            }
            catch(Exception ex) {
                if (ex.Message.Contains("cannot access the file"))
                {
                    MessageBox.Show("File 1 is open and cannot be read. \r\n\r\nPlease close file 1 and try again.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Console.WriteLine("Exception Reading CSV File 1: {0} ...", ex.Message);
                }
                if (ex.Message.Contains("File not in Antz Format"))
                {
                    MessageBox.Show("File 1 is not in the correct Antz Node Format. \r\n\r\nPlease ensure that your file is a valid Antz Formatted file.", "Error", MessageBoxButtons.OK,MessageBoxIcon.Error);
                    Console.WriteLine("Exception Wrong format File 1: {0} ...", ex.Message);
                }
                

                retValue = false;
                return retValue;
            }
            finally
            {
                // nothing yet
            }
            

            return retValue;
        }

        private void analyzeGlyphDiffs(List<sgxAntz> nL1, List<sgxAntz> nL2)
        {
            bool numLevelsIsEqual = false;
            int childNumber;
            int childBranchLevel;
            Console.WriteLine("NodeList 1 length = " + nL1.Count);
            if (nL1.Count == nL2.Count) numLevelsIsEqual = true;
            if (numLevelsIsEqual)
            {
                for (int i = 0; i < nL1.Count; i++)
                {
                    // start comparing things
                    if (i == 0) // L0 glyph
                    {
                        if (nL1[i].geometry - nL2[i].geometry != 0) ampzarray[0, 0].isDiff_geometry = true;
                        if (nL1[i].translate_x - nL2[i].translate_x != 0) ampzarray[0, 0].isDiff_translate_x = true;
                        if (nL1[i].translate_y - nL2[i].translate_y != 0) ampzarray[0, 0].isDiff_translate_y = true;
                        if (nL1[i].translate_z - nL2[i].translate_z != 0) ampzarray[0, 0].isDiff_translate_z = true;
                        if (nL1[i].scale_x - nL2[i].scale_x != 0) ampzarray[0, 0].isDiff_scale_x = true;
                        if (nL1[i].scale_y - nL2[i].scale_y != 0) ampzarray[0, 0].isDiff_scale_y = true;
                        if (nL1[i].scale_z - nL2[i].scale_z != 0) ampzarray[0, 0].isDiff_scale_z = true;
                        if (nL1[i].color_a - nL2[i].color_a != 0) ampzarray[0, 0].isDiff_color_a = true;
                        if (nL1[i].color_r - nL2[i].color_r != 0) ampzarray[0, 0].isDiff_color_r = true;
                        if (nL1[i].color_g - nL2[i].color_g != 0) ampzarray[0, 0].isDiff_color_g = true;
                        if (nL1[i].color_b - nL2[i].color_b != 0) ampzarray[0, 0].isDiff_color_b = true;
                        if (nL1[i].topo - nL2[i].topo != 0) ampzarray[0, 0].isDiff_topology = true;
                        if (nL1[i].ratio - nL2[i].ratio != 0) ampzarray[0, 0].isDiff_ratio = true;
                        
                    }
                    else // L1 or above
                    {
                        // firgure out which child number this glyph is 
                        childNumber = nL1[i].child_id;
                        childBranchLevel = nL1[i].branch_level;
                        if (nL1[i].geometry - nL2[i].geometry != 0) ampzarray[childBranchLevel, childNumber].isDiff_geometry = true;
                        if (nL1[i].translate_x - nL2[i].translate_x != 0) ampzarray[childBranchLevel, childNumber].isDiff_translate_x = true;
                        if (nL1[i].translate_y - nL2[i].translate_y != 0) ampzarray[childBranchLevel, childNumber].isDiff_translate_y = true;
                        if (nL1[i].translate_z - nL2[i].translate_z != 0) ampzarray[childBranchLevel, childNumber].isDiff_translate_z = true;
                        if (nL1[i].scale_x - nL2[i].scale_x != 0) ampzarray[childBranchLevel, childNumber].isDiff_scale_x = true;
                        if (nL1[i].scale_y - nL2[i].scale_y != 0) ampzarray[childBranchLevel, childNumber].isDiff_scale_y = true;
                        if (nL1[i].scale_z - nL2[i].scale_z != 0) ampzarray[childBranchLevel, childNumber].isDiff_scale_z = true;
                        if (nL1[i].color_a - nL2[i].color_a != 0) ampzarray[childBranchLevel, childNumber].isDiff_color_a = true;
                        if (nL1[i].color_r - nL2[i].color_r != 0) ampzarray[childBranchLevel, childNumber].isDiff_color_r = true;
                        if (nL1[i].color_g - nL2[i].color_g != 0) ampzarray[childBranchLevel, childNumber].isDiff_color_g = true;
                        if (nL1[i].color_b - nL2[i].color_b != 0) ampzarray[childBranchLevel, childNumber].isDiff_color_b = true;
                        if (nL1[i].topo - nL2[i].topo != 0) ampzarray[childBranchLevel, childNumber].isDiff_topology = true;
                        if (nL1[i].ratio - nL2[i].ratio != 0) ampzarray[childBranchLevel, childNumber].isDiff_ratio = true;
                    }
                }
                Program.OK_to_Diff_Map = true;
            }
        }

        /// <summary>
        /// Given an antz formated data string, returns an sgxAntz object.
        /// </summary>
        /// <param name="csvResult1"></param>
        /// <returns>An antz object based on the input string.</returns>
        private sgxAntz parseAntzNode(CsvReader csvResult1)
        {
            sgxAntz node = new sgxAntz();

            node.id = Convert.ToInt16(csvResult1.GetField("id"));
            node.type = Convert.ToInt16(csvResult1.GetField("type"));
            node.data = Convert.ToInt16(csvResult1.GetField("data"));
            node.selected = Convert.ToInt16(csvResult1.GetField("selected"));
            node.parent_id = Convert.ToInt16(csvResult1.GetField("parent_id"));
            node.branch_level = Convert.ToInt16(csvResult1.GetField("branch_level"));
            node.child_id = Convert.ToInt16(csvResult1.GetField("child_id"));
            node.child_index = Convert.ToInt16(csvResult1.GetField("child_index"));
            node.child_count = Convert.ToInt16(csvResult1.GetField("child_count"));
            node.ch_input_id = Convert.ToInt16(csvResult1.GetField("ch_input_id"));
            node.ch_output_id = Convert.ToInt16(csvResult1.GetField("ch_output_id"));
            node.ch_last_updated = Convert.ToInt16(csvResult1.GetField("ch_last_updated"));
            node.average = Convert.ToInt16(csvResult1.GetField("average"));
            node.sample = Convert.ToInt16(csvResult1.GetField("sample"));
            node.aux_a_x = Convert.ToInt16(csvResult1.GetField("aux_a_x"));
            node.aux_a_z = Convert.ToInt16(csvResult1.GetField("aux_a_z"));
            node.aux_b_x = Convert.ToInt16(csvResult1.GetField("aux_b_x"));
            node.aux_b_y = Convert.ToInt16(csvResult1.GetField("aux_b_y"));
            node.aux_b_z = Convert.ToInt16(csvResult1.GetField("aux_b_z"));
            node.color_shift = Convert.ToSingle(csvResult1.GetField("color_shift"));
            node.rotate_vec_x = Convert.ToSingle(csvResult1.GetField("rotate_vec_x"));
            node.rotate_vec_y = Convert.ToSingle(csvResult1.GetField("rotate_vec_y"));
            node.rotate_vec_z = Convert.ToSingle(csvResult1.GetField("rotate_vec_z"));
            node.rotate_vec_s = Convert.ToSingle(csvResult1.GetField("rotate_vec_s"));
            node.scale_x = Convert.ToSingle(csvResult1.GetField("scale_x"));
            node.scale_y = Convert.ToSingle(csvResult1.GetField("scale_y"));
            node.scale_z = Convert.ToSingle(csvResult1.GetField("scale_z"));
            node.translate_x = Convert.ToSingle(csvResult1.GetField("translate_x"));
            node.translate_y = Convert.ToSingle(csvResult1.GetField("translate_y"));
            node.translate_z = Convert.ToSingle(csvResult1.GetField("translate_z"));
            node.tag_offset_x = Convert.ToSingle(csvResult1.GetField("tag_offset_x"));
            node.tag_offset_y = Convert.ToSingle(csvResult1.GetField("tag_offset_y"));
            node.tag_offset_z = Convert.ToSingle(csvResult1.GetField("tag_offset_z"));
            node.rotate_rate_x = Convert.ToSingle(csvResult1.GetField("rotate_rate_x"));
            node.rotate_rate_y = Convert.ToSingle(csvResult1.GetField("rotate_rate_y"));
            node.rotate_rate_z = Convert.ToSingle(csvResult1.GetField("rotate_rate_z"));
            node.rotate_x = Convert.ToSingle(csvResult1.GetField("rotate_x"));
            node.rotate_y = Convert.ToSingle(csvResult1.GetField("rotate_y"));
            node.rotate_z = Convert.ToSingle(csvResult1.GetField("rotate_z"));
            node.scale_rate_x = Convert.ToSingle(csvResult1.GetField("scale_rate_x"));
            node.scale_rate_y = Convert.ToSingle(csvResult1.GetField("scale_rate_y"));
            node.scale_rate_z = Convert.ToSingle(csvResult1.GetField("scale_rate_z"));
            node.translate_rate_x = Convert.ToSingle(csvResult1.GetField("translate_rate_x"));
            node.translate_rate_y = Convert.ToSingle(csvResult1.GetField("translate_rate_y"));
            node.translate_rate_z = Convert.ToSingle(csvResult1.GetField("translate_rate_z"));
            node.translate_vec_x = Convert.ToSingle(csvResult1.GetField("translate_vec_x"));
            node.translate_vec_y = Convert.ToSingle(csvResult1.GetField("translate_vec_y"));
            node.translate_vec_z = Convert.ToSingle(csvResult1.GetField("translate_vec_z"));
            node.shader = Convert.ToInt16(csvResult1.GetField("shader"));
            node.geometry = Convert.ToInt16(csvResult1.GetField("geometry"));
            node.line_width = Convert.ToSingle(csvResult1.GetField("line_width"));
            node.point_size = Convert.ToSingle(csvResult1.GetField("point_size"));
            node.ratio = Convert.ToSingle(csvResult1.GetField("ratio"));
            node.color_index = Convert.ToInt16(csvResult1.GetField("color_index"));
            node.color_r = Convert.ToInt16(csvResult1.GetField("color_r"));
            node.color_g = Convert.ToInt16(csvResult1.GetField("color_g"));
            node.color_b = Convert.ToInt16(csvResult1.GetField("color_b"));
            node.color_a = Convert.ToInt16(csvResult1.GetField("color_a"));
            node.color_fade = Convert.ToInt16(csvResult1.GetField("color_fade"));
            node.texture_id = Convert.ToInt16(csvResult1.GetField("texture_id"));
            node.hide = Convert.ToInt16(csvResult1.GetField("hide"));
            node.freeze = Convert.ToInt16(csvResult1.GetField("freeze"));
            node.topo = Convert.ToInt16(csvResult1.GetField("topo"));
            node.facet = Convert.ToInt16(csvResult1.GetField("facet"));
            node.auto_zoom_x = Convert.ToInt16(csvResult1.GetField("auto_zoom_x"));
            node.auto_zoom_y = Convert.ToInt16(csvResult1.GetField("auto_zoom_y"));
            node.auto_zoom_z = Convert.ToInt16(csvResult1.GetField("auto_zoom_z"));
            node.trigger_hi_x = Convert.ToInt16(csvResult1.GetField("trigger_hi_x"));
            node.trigger_hi_y = Convert.ToInt16(csvResult1.GetField("trigger_hi_y"));
            node.trigger_hi_z = Convert.ToInt16(csvResult1.GetField("trigger_hi_z"));
            node.trigger_lo_x = Convert.ToInt16(csvResult1.GetField("trigger_lo_x"));
            node.trigger_lo_y = Convert.ToInt16(csvResult1.GetField("trigger_lo_y"));
            node.trigger_lo_z = Convert.ToInt16(csvResult1.GetField("trigger_lo_z"));
            node.set_hi_x = Convert.ToSingle(csvResult1.GetField("set_hi_x"));
            node.set_hi_y = Convert.ToSingle(csvResult1.GetField("set_hi_y"));
            node.set_hi_z = Convert.ToSingle(csvResult1.GetField("set_hi_z"));
            node.set_lo_x = Convert.ToSingle(csvResult1.GetField("set_lo_x"));
            node.set_lo_y = Convert.ToSingle(csvResult1.GetField("set_lo_y"));
            node.set_lo_z = Convert.ToSingle(csvResult1.GetField("set_lo_z"));
            node.proximity_x = Convert.ToSingle(csvResult1.GetField("proximity_x"));
            node.proximity_y = Convert.ToSingle(csvResult1.GetField("proximity_y"));
            node.proximity_z = Convert.ToSingle(csvResult1.GetField("proximity_z"));
            node.proximity_mode_x = Convert.ToInt16(csvResult1.GetField("proximity_mode_x"));
            node.proximity_mode_y = Convert.ToInt16(csvResult1.GetField("proximity_mode_y"));
            node.proximity_mode_z = Convert.ToInt16(csvResult1.GetField("proximity_mode_z"));
            node.segments_x = Convert.ToInt16(csvResult1.GetField("segments_x"));
            node.segments_y = Convert.ToInt16(csvResult1.GetField("segments_y"));
            node.segments_z = Convert.ToInt16(csvResult1.GetField("segments_z"));
            node.tag_mode = Convert.ToInt16(csvResult1.GetField("tag_mode"));
            node.format_id = Convert.ToInt16(csvResult1.GetField("format_id"));
            node.table_id = Convert.ToInt16(csvResult1.GetField("table_id"));
            node.record_id = Convert.ToInt16(csvResult1.GetField("record_id"));
            node.size = Convert.ToInt16(csvResult1.GetField("size"));

            return node;
        }

        /// <summary>
        /// Check to see if the string passed in (inHeader) matches the Antz header format and length
        /// </summary>
        /// <param name="inHeader"></param>
        /// <returns> true if everything matches</returns>
        private bool check_if_file_in_Antz_Format(string[] inHeader)
        {
            bool DaTruth = false;

            // Check length of header
            if (inHeader.Length == 94) { DaTruth = true; } else return false;
            // Now Check every single header fields
            if (inHeader[0].Contains("id")) { DaTruth = true; } else return false;
            if (inHeader[1].Contains("type")) { DaTruth = true; } else return false;
            if (inHeader[2].Contains("data")) { DaTruth = true; } else return false;
            if (inHeader[3].Contains("selected")) { DaTruth = true; } else return false;
            if (inHeader[4].Contains("parent_id")) { DaTruth = true; } else return false;
            if (inHeader[5].Contains("branch_level")) { DaTruth = true; } else return false;
            if (inHeader[6].Contains("child_id")) { DaTruth = true; } else return false;
            if (inHeader[7].Contains("child_index")) { DaTruth = true; } else return false;
            if (inHeader[8].Contains("child_count")) { DaTruth = true; } else return false;
            if (inHeader[9].Contains("ch_input_id")) { DaTruth = true; } else return false;
            if (inHeader[10].Contains("ch_output_id")) { DaTruth = true; } else return false;
            if (inHeader[11].Contains("ch_last_updated")) { DaTruth = true; } else return false;
            if (inHeader[12].Contains("average")) { DaTruth = true; } else return false;
            if (inHeader[13].Contains("sample")) { DaTruth = true; } else return false;
            if (inHeader[14].Contains("aux_a_x")) { DaTruth = true; } else return false;
            if (inHeader[15].Contains("aux_a_y")) { DaTruth = true; } else return false;
            if (inHeader[16].Contains("aux_a_z")) { DaTruth = true; } else return false;
            if (inHeader[17].Contains("aux_b_x")) { DaTruth = true; } else return false;
            if (inHeader[18].Contains("aux_b_y")) { DaTruth = true; } else return false;
            if (inHeader[19].Contains("aux_b_z")) { DaTruth = true; } else return false;
            if (inHeader[20].Contains("color_shift")) { DaTruth = true; } else return false;
            if (inHeader[21].Contains("rotate_vec_x")) { DaTruth = true; } else return false;
            if (inHeader[22].Contains("rotate_vec_y")) { DaTruth = true; } else return false;
            if (inHeader[23].Contains("rotate_vec_z")) { DaTruth = true; } else return false;
            if (inHeader[24].Contains("rotate_vec_s")) { DaTruth = true; } else return false;
            if (inHeader[25].Contains("scale_x")) { DaTruth = true; } else return false;
            if (inHeader[26].Contains("scale_y")) { DaTruth = true; } else return false;
            if (inHeader[27].Contains("scale_z")) { DaTruth = true; } else return false;
            if (inHeader[28].Contains("translate_x")) { DaTruth = true; } else return false;
            if (inHeader[29].Contains("translate_y")) { DaTruth = true; } else return false;
            if (inHeader[30].Contains("translate_z")) { DaTruth = true; } else return false;
            if (inHeader[31].Contains("tag_offset_x")) { DaTruth = true; } else return false;
            if (inHeader[32].Contains("tag_offset_y")) { DaTruth = true; } else return false;
            if (inHeader[33].Contains("tag_offset_z")) { DaTruth = true; } else return false;
            if (inHeader[34].Contains("rotate_rate_x")) { DaTruth = true; } else return false;
            if (inHeader[35].Contains("rotate_rate_y")) { DaTruth = true; } else return false;
            if (inHeader[36].Contains("rotate_rate_z")) { DaTruth = true; } else return false;
            if (inHeader[37].Contains("rotate_x")) { DaTruth = true; } else return false;
            if (inHeader[38].Contains("rotate_y")) { DaTruth = true; } else return false;
            if (inHeader[39].Contains("rotate_z")) { DaTruth = true; } else return false;
            if (inHeader[40].Contains("scale_rate_x")) { DaTruth = true; } else return false;
            if (inHeader[41].Contains("scale_rate_y")) { DaTruth = true; } else return false;
            if (inHeader[42].Contains("scale_rate_z")) { DaTruth = true; } else return false;
            if (inHeader[43].Contains("translate_rate_x")) { DaTruth = true; } else return false;
            if (inHeader[44].Contains("translate_rate_y")) { DaTruth = true; } else return false;
            if (inHeader[45].Contains("translate_rate_z")) { DaTruth = true; } else return false;
            if (inHeader[46].Contains("translate_vec_x")) { DaTruth = true; } else return false;
            if (inHeader[47].Contains("translate_vec_y")) { DaTruth = true; } else return false;
            if (inHeader[48].Contains("translate_vec_z")) { DaTruth = true; } else return false;
            if (inHeader[49].Contains("shader")) { DaTruth = true; } else return false;
            if (inHeader[50].Contains("geometry")) { DaTruth = true; } else return false;
            if (inHeader[51].Contains("line_width")) { DaTruth = true; } else return false;
            if (inHeader[52].Contains("point_size")) { DaTruth = true; } else return false;
            if (inHeader[53].Contains("ratio")) { DaTruth = true; } else return false;
            if (inHeader[54].Contains("color_index")) { DaTruth = true; } else return false;
            if (inHeader[55].Contains("color_r")) { DaTruth = true; } else return false;
            if (inHeader[56].Contains("color_g")) { DaTruth = true; } else return false;
            if (inHeader[57].Contains("color_b")) { DaTruth = true; } else return false;
            if (inHeader[58].Contains("color_a")) { DaTruth = true; } else return false;
            if (inHeader[59].Contains("color_fade")) { DaTruth = true; } else return false;
            if (inHeader[60].Contains("texture_id")) { DaTruth = true; } else return false;
            if (inHeader[61].Contains("hide")) { DaTruth = true; } else return false;
            if (inHeader[62].Contains("freeze")) { DaTruth = true; } else return false;
            if (inHeader[63].Contains("topo")) { DaTruth = true; } else return false;
            if (inHeader[64].Contains("facet")) { DaTruth = true; } else return false;
            if (inHeader[65].Contains("auto_zoom_x")) { DaTruth = true; } else return false;
            if (inHeader[66].Contains("auto_zoom_y")) { DaTruth = true; } else return false;
            if (inHeader[67].Contains("auto_zoom_z")) { DaTruth = true; } else return false;
            if (inHeader[68].Contains("trigger_hi_x")) { DaTruth = true; } else return false;
            if (inHeader[69].Contains("trigger_hi_y")) { DaTruth = true; } else return false;
            if (inHeader[70].Contains("trigger_hi_z")) { DaTruth = true; } else return false;
            if (inHeader[71].Contains("trigger_lo_x")) { DaTruth = true; } else return false;
            if (inHeader[72].Contains("trigger_lo_y")) { DaTruth = true; } else return false;
            if (inHeader[73].Contains("trigger_lo_z")) { DaTruth = true; } else return false;
            if (inHeader[74].Contains("set_hi_x")) { DaTruth = true; } else return false;
            if (inHeader[75].Contains("set_hi_y")) { DaTruth = true; } else return false;
            if (inHeader[76].Contains("set_hi_z")) { DaTruth = true; } else return false;
            if (inHeader[77].Contains("set_lo_x")) { DaTruth = true; } else return false;
            if (inHeader[78].Contains("set_lo_y")) { DaTruth = true; } else return false;
            if (inHeader[79].Contains("set_lo_z")) { DaTruth = true; } else return false;
            if (inHeader[80].Contains("proximity_x")) { DaTruth = true; } else return false;
            if (inHeader[81].Contains("proximity_y")) { DaTruth = true; } else return false;
            if (inHeader[82].Contains("proximity_z")) { DaTruth = true; } else return false;
            if (inHeader[83].Contains("proximity_mode_x")) { DaTruth = true; } else return false;
            if (inHeader[84].Contains("proximity_mode_y")) { DaTruth = true; } else return false;
            if (inHeader[85].Contains("proximity_mode_z")) { DaTruth = true; } else return false;
            if (inHeader[86].Contains("segments_x")) { DaTruth = true; } else return false;
            if (inHeader[87].Contains("segments_y")) { DaTruth = true; } else return false;
            if (inHeader[88].Contains("segments_z")) { DaTruth = true; } else return false;
            if (inHeader[89].Contains("tag_mode")) { DaTruth = true; } else return false;
            if (inHeader[90].Contains("format_id")) { DaTruth = true; } else return false;
            if (inHeader[91].Contains("table_id")) { DaTruth = true; } else return false;
            if (inHeader[92].Contains("record_id")) { DaTruth = true; } else return false;
            if (inHeader[93].Contains("size")) { DaTruth = true; } else return false; 

            // Since we made it this far, the file is verified in the correct antz format

            return DaTruth;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This action will reset all the level and you will loose your curent mappings. Do you wish to continue?", "Are you certain?",MessageBoxButtons.YesNoCancel);
        }

        private void stepItem4_Click(object sender, EventArgs e)
        {
            if (Program.OK_to_Diff_Map)
            {
                MessageBox.Show("ok to diff map!");
                // set the number of glyphs per level
                // 2- call do_Levels_Map_Diff

            }
        }

        private void wfMainStep3_Click(object sender, EventArgs e)
        {

        }

        private void wfMainStep4_Click(object sender, EventArgs e)
        {

        }

        private void cbDiffGlyphs_CheckedChanged(object sender, EventArgs e)
        {
            string result1 = null;
            foreach (Control control in this.gpGlyphSource.Controls)
            {
                if (control is DevComponents.DotNetBar.Controls.CheckBoxX   )
                {
                    DevComponents.DotNetBar.Controls.CheckBoxX radio = control as DevComponents.DotNetBar.Controls.CheckBoxX;
                    if (radio.Checked)
                    {
                        result1 = radio.Text;
                        if ((string)radio.Tag == "1")
                        {
                            Program.gDiffUse1file = true;
                        }
                        if ((string)radio.Tag == "2")
                        {
                            Program.gDiffUse1file = false;
                        }
                        if (Program.gDiffUse1file)
                        {
                            btX_AntzFile1.Visible = true;
                            btX_AntzFile1.Enabled = true;
                            txtBox_File1.Visible = true;
                            txtBox_File1.Enabled = true;
                            btX_AntzFile2.Visible = false;
                            btX_AntzFile2.Enabled = false;
                            txtBox_File2.Visible = false;
                            txtBox_File2.Enabled = false;
                        }
                        else
                        {
                            btX_AntzFile1.Visible = true;
                            btX_AntzFile1.Enabled = true;
                            txtBox_File1.Visible = true;
                            txtBox_File1.Enabled = true;
                            btX_AntzFile2.Visible = true;
                            btX_AntzFile2.Enabled = true;
                            txtBox_File2.Visible = true;
                            txtBox_File2.Enabled = true;
                        }
                    }
                }
            }
            statusBox.Text = "You selected: " + result1;
            

        }
        private void onDropDownChange(object sender, EventArgs e)
        {
            wfMainStep3.Value = 99;
            statusBox.Text = "ComboBox fired event:  " ;
        }

    }    // Form1
}  //IngestionEngine
