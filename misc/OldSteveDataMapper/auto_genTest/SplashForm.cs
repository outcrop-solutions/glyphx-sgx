using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IngestionEngine
{
    public partial class SplashForm : Form
    {
        public Dictionary<int, string> MysticalSayings = new Dictionary<int, string>();
        public SplashForm()
        {
            InitializeComponent();
            Random random = new Random();
            
            MysticalSayings.Add( 1,"Hedonistic Barbarians");
            MysticalSayings.Add( 2,"Quirky Qwirvy");
            MysticalSayings.Add( 3,"State Specific Scientists");
            MysticalSayings.Add( 4,"Angry Nerds");
            MysticalSayings.Add( 5,"Mild Mannered Canadians");
            MysticalSayings.Add( 6,"Intelectual Freedom Fighters");
            MysticalSayings.Add( 7,"Cowles Mountain Sunrises");
            MysticalSayings.Add( 8,"Poly Morpic Topo Cromo Sonics");
            MysticalSayings.Add( 9,"Tangerine Dreamers");
            MysticalSayings.Add(10,"the Pseudo Feds");
            MysticalSayings.Add(11,"Tropical Rainforest Readings");
            MysticalSayings.Add(12,"Part Time Zen Master");
            MysticalSayings.Add(13,"Center for Really Neat Research");
            MysticalSayings.Add(14,"Mandelbrot Bothers");
            MysticalSayings.Add(15,"Adopted Hippies");
            MysticalSayings.Add(16,"the Tribe You Didnt Know You Belong To");
            MysticalSayings.Add(17,"Amazon Goddeses");
            MysticalSayings.Add(18,"Black Rock City Residents");
            MysticalSayings.Add(19,"Cybernarium Refugees");
            MysticalSayings.Add(20,"Sunset at Torrey Pines");


            int randomCheeze = random.Next(1, 20);

            CheezyTagLineLabel.Text = "Powered by " + MysticalSayings[randomCheeze];
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            progressBar1.Increment(1);
            if (progressBar1.Value == 100) timer1.Stop();
        }
    
    }

}
