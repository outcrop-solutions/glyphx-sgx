using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ColorMine;

namespace IngestionEngine
{
    public partial class Palette_Manager : Form
    {
        public bool gotColor1 = false;
        public bool gotColor2 = false;
        public Palette_Manager()
        {
            InitializeComponent();
            pal_List.DataSource = IngestionEngine.Program.PaletteList.Keys.ToList();
            pal_List.DisplayMember = Program.PaletteList.Keys.ToString();
            
        }

        public void DrawGradientPallet(){
            System.Drawing.SolidBrush myBrush = new System.Drawing.SolidBrush(System.Drawing.Color.Red);
            LinearGradientBrush linGrBrush = new LinearGradientBrush(
               new Point(10, 0),
               new Point(10, 200),
               Color.FromArgb(255, HighColorButton.BackColor),   // Opaque red
               Color.FromArgb(255, LowColorButton.BackColor));  // Opaque blue


            System.Drawing.Graphics palGraf = palette_man.CreateGraphics();
            palGraf.FillRectangle(linGrBrush, new Rectangle(0, 0, 200, 300));

            myBrush.Dispose();
            linGrBrush.Dispose();
            palGraf.Dispose();
        }
        public void DrawBlankPallet()
        {
            LinearGradientBrush linGrBrush = new LinearGradientBrush(
               new Point(0, 0),
               new Point(0, palette_man.Size.Height),
               Color.FromArgb(255, System.Drawing.Color.White),   //  White
               Color.FromArgb(255, System.Drawing.Color.GhostWhite));  // GhostWhite


            System.Drawing.Graphics palGraf = palette_man.CreateGraphics();
            palGraf.FillRectangle(linGrBrush, new Rectangle(0, 0, palette_man.Size.Width, palette_man.Size.Height));

            linGrBrush.Dispose();
            palGraf.Dispose();
        }
        private void StartButton_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                gotColor1 = true;
                LowColorButton.BackColor = colorDialog1.Color;
                LowColorButton.ForeColor = ContrastColor(colorDialog1.Color);
                if (gotColor1 && gotColor2)
                {
                    DrawGradientPallet();
                    labelLow.Visible = true;
                    LowValueSlider.Visible = true;
                    labelHigh.Visible = true;
                    HighValueSlider.Visible = true;
                    paletteSave.Enabled = true;
                    paletteName.Enabled = true;
                } 
            }
        }

        private void EndButton_Click(object sender, EventArgs e)
        {
            if (colorDialog1.ShowDialog() == DialogResult.OK)
            {
                gotColor2 = true;
                HighColorButton.BackColor = colorDialog1.Color;
                HighColorButton.ForeColor = ContrastColor(colorDialog1.Color);
                if (gotColor1 && gotColor2) 
                { 
                    DrawGradientPallet();
                    labelLow.Visible = true;
                    LowValueSlider.Visible = true;
                    labelHigh.Visible = true;
                    HighValueSlider.Visible = true;
                    paletteSave.Enabled = true;
                    paletteName.Enabled = true;
                } 
            }
        }

        Color ContrastColor(Color color)
        {
            int d = 0;

            // Counting the perceptive luminance - human eye favors green color... 
            double a = 1 - (0.299 * color.R + 0.587 * color.G + 0.114 * color.B) / 255;

            if (a < 0.5)
                d = 0; // bright colors - black font
            else
                d = 255; // dark colors - white font

            return Color.FromArgb(d, d, d);
        }

        private void LowValueSlider_Scroll(object sender, EventArgs e)
        {
            if (LowValueSlider.Value < HighValueSlider.Value - 1)
                labelLow.Text = LowValueSlider.Value.ToString();
            else
            {
                LowValueSlider.Value = HighValueSlider.Value - 1;
                labelLow.Text = (HighValueSlider.Value - 1).ToString();
            }
                
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            if (HighValueSlider.Value >= LowValueSlider.Value + 1)
                labelHigh.Text = HighValueSlider.Value.ToString();
            else
            {
                HighValueSlider.Value = LowValueSlider.Value + 1;
                labelHigh.Text = (LowValueSlider.Value + 1).ToString();
            }
                
        }

        private void palette_man_Paint(object sender, PaintEventArgs e)
        {
            if (!gotColor1 && !gotColor2)
                DrawBlankPallet();
            else
                DrawDemoPallet();
        }

        private void paletteShow_Click(object sender, EventArgs e)
        {
            string usePalette = pal_List.SelectedItem.ToString();
            //MessageBox.Show("Show me " +  usePalette + " Palette!", "Show Palette");
            LowColorButton.BackColor = Program.PaletteList[usePalette].lowColor;
            HighColorButton.BackColor = Program.PaletteList[usePalette].hiColor;
            gotColor1 = gotColor2 = true;
            labelLow.Text = Program.PaletteList[usePalette].lowNumber.ToString();
            labelHigh.Text = Program.PaletteList[usePalette].hiNumber.ToString();
            LowColorButton.ForeColor = ContrastColor(LowColorButton.BackColor);
            HighColorButton.ForeColor = ContrastColor(HighColorButton.BackColor);
            LowValueSlider.Value = Program.PaletteList[usePalette].lowNumber;
            HighValueSlider.Value = Program.PaletteList[usePalette].hiNumber;
            DrawGradientPallet();
            paletteSave.Enabled = true;
            paletteName.Enabled = true;
            paletteName.Text = usePalette;
            paletteDelete.Enabled = true; 
            labelLow.Visible = true;
            LowValueSlider.Visible = true;
            labelHigh.Visible = true;
            HighValueSlider.Visible = true;
            paletteSave.Enabled = true;
            paletteName.Enabled = true;

        }

        private void paletteSave_Click(object sender, EventArgs e)
        {
            if (paletteName.Text == "" || paletteName.Text == null)
            {
                MessageBox.Show("Please enter a Name for your palette.","Whoopsie");
                paletteName.Focus(); 
            }
            else
            {
                Palette_Glyph_Class defaultnew = new Palette_Glyph_Class();

                defaultnew.lowColor = LowColorButton.BackColor;
                defaultnew.lowNumber = LowValueSlider.Value;
                defaultnew.hiColor = HighColorButton.BackColor;
                defaultnew.hiNumber = HighValueSlider.Value;
                defaultnew.name = paletteName.Text;
                if (Program.PaletteList.ContainsKey(defaultnew.name))
                {
                    DialogResult dialogResult = MessageBox.Show("Do you want to overwrite the existing palette?", "Warning: Duplicate Name", MessageBoxButtons.YesNo);
                    if(dialogResult == DialogResult.Yes)
                    {// remove then re-add new 
                            Program.PaletteList.Remove(defaultnew.name);
                            Program.PaletteList.Add(defaultnew.name, defaultnew);
                            pal_List.DataSource = IngestionEngine.Program.PaletteList.Keys.ToList();
                            pal_List.DisplayMember = Program.PaletteList.Keys.ToString();
                    }
                    else
                    {// bail out
                        paletteName.Focus();
                        return;
                        //Program.PaletteList.Add(defaultnew.name, defaultnew);
                    }
                }
                else
                {
                    Program.PaletteList.Add(defaultnew.name, defaultnew);
                    pal_List.DataSource = Program.PaletteList.Keys.ToList();
                    pal_List.DisplayMember = Program.PaletteList.Keys.ToString();
                    
                }
                
            }
        }

        private void palette_man_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            if (e.Y >= (palette_man.Size.Height / 2))
                StartButton_Click( sender,  e);
            else
                EndButton_Click(sender, e);
        }

        private void pal_man_DoubleClick(object sender, MouseEventArgs e)
        {
            paletteShow_Click(sender, e);
        }

        private void paletteName_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Enter)
            {
                paletteSave_Click(sender, e);
            }
        }

        private void paletteDelete_Click(object sender, EventArgs e)
        {
            if (pal_List.SelectedItem != null)
            {
                DialogResult dialogResult = MessageBox.Show("Are you sure you want to delete palate: " + pal_List.SelectedItem.ToString()+" from the list?",
                    "Confirm Delete", MessageBoxButtons.YesNo);
                if (dialogResult == DialogResult.Yes)
                {
                    Program.PaletteList.Remove(pal_List.SelectedItem.ToString());
                    pal_List.DataSource = IngestionEngine.Program.PaletteList.Keys.ToList();
                    pal_List.DisplayMember = Program.PaletteList.Keys.ToString();
                }

            }
        }

        private void Palette_Manager_FormClosing(object sender, FormClosingEventArgs e)
        {  // save palette on exit
            Program.DP_list_items.Clear();
            Program.DP_list_items.Add("", "");
            foreach (string Kpals in Program.PaletteList.Keys)
            {
                Program.DP_list_items.Add(Kpals.ToString(), Kpals.ToString());
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (!gotColor1 && !gotColor2)
                DrawBlankPallet();
            else
                DrawDemoPallet();
        }

        Double mylerp(double a, double b, double t)
        {
            return  (a * (1 - t) + b * t);
        }
        public void DrawDemoPallet()
        {
            System.Drawing.Pen myBrush = new System.Drawing.Pen(System.Drawing.Color.Red);
            LinearGradientBrush linGrBrush = new LinearGradientBrush(
               new Point(10, 0),
               new Point(10, 200),
               Color.FromArgb(255, HighColorButton.BackColor),   // Opaque red
               Color.FromArgb(255, LowColorButton.BackColor));  // Opaque blue

            ColorMine.ColorSpaces.Rgb colr1 = new  ColorMine.ColorSpaces.Rgb();
            ColorMine.ColorSpaces.Rgb colr2 = new ColorMine.ColorSpaces.Rgb();
            ColorMine.ColorSpaces.Rgb Frgb = new ColorMine.ColorSpaces.Rgb();
            colr2.R = LowColorButton.BackColor.R; colr2.G = LowColorButton.BackColor.G; colr2.B = LowColorButton.BackColor.B;
            colr1.R = HighColorButton.BackColor.R; colr1.G = HighColorButton.BackColor.G; colr1.B = HighColorButton.BackColor.B;

            ColorMine.ColorSpaces.Lch hsv1 = colr1.To<ColorMine.ColorSpaces.Lch>();
            ColorMine.ColorSpaces.Lch hsv2 = colr2.To<ColorMine.ColorSpaces.Lch>();
            ColorMine.ColorSpaces.Lch hfinl = new ColorMine.ColorSpaces.Lch();
            
            
            System.Drawing.Graphics palGraf = palette_man.CreateGraphics();
            //palGraf.FillRectangle(linGrBrush, new Rectangle(0, 0, 200, 300));
            double step = Convert.ToDouble(1.0 / 300.0);
            double t = step;
            int fR, fG, fB;
            Console.WriteLine("J={0} H= {1}, S={2}, B={3}, R={4} G={5} B={6}, t={7}", 300, hsv1.H, hsv1.C, hsv1.L, colr1.R, colr1.G, colr1.B, step);
                
            for (int jj = 300; jj > 0; jj--)
            {
                t = step * jj;
                hfinl.H = mylerp(hsv1.H, hsv2.H, t);
                hfinl.C = mylerp(hsv1.C, hsv2.C, t);
                hfinl.L = mylerp(hsv1.L, hsv2.L,  t);
                Frgb = hfinl.To<ColorMine.ColorSpaces.Rgb>();

                fR = (int)Frgb.R;
                fG = (int)Frgb.G;
                fB = (int)Frgb.B;
                if ( jj == 150) Console.WriteLine("J={0} H= {1}, S={2}, B={3}, R={4} G={5} B={6}, t={7}", jj, hfinl.H, hfinl.C, hfinl.L, fR, fG, fB, t);
                myBrush.Color=  Color.FromArgb(255, fR, fG, fB);
                palGraf.DrawLine(myBrush,0,jj,200,jj);
            }
            Console.WriteLine("J={0} H= {1}, S={2}, B={3}, R={4} G={5} B={6}, t={7}", 0, hsv2.H, hsv2.C, hsv2.L, colr2.R, colr2.G, colr2.B, step);
                
            Frgb = hsv2.To<ColorMine.ColorSpaces.Rgb>();
            myBrush.Color = Color.FromArgb(255, (int)Frgb.R, (int)Frgb.G, (int)Frgb.B);
            palGraf.DrawLine(myBrush, 0, 0, 200, 0);
            myBrush.Dispose();
            linGrBrush.Dispose();
            palGraf.Dispose();
        }

    }
}
