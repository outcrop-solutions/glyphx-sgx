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
    public partial class AutoScale_Defaults : Form
    {
        public AutoScale_Defaults()
        {
            InitializeComponent();
        }

        private void as_x_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["translate_x_min"] = (double) as_x_min.Value;
        }

        private void as_x_max_ValueChanged(object sender, EventArgs e)
        {
            Program.AutoScalerDefaults["translate_x_max"] = (double)as_x_max.Value;
        }

        private void as_y_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["translate_y_min"] = (double)as_y_min.Value;
        }

        private void as_y_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["translate_y_max"] = (double)as_y_max.Value;
        }

        private void as_z_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["translate_z_min"] = (double)as_z_min.Value;
        }

        private void as_z_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["translate_z_max"] = (double)as_z_max.Value;
        }

        private void as_cindex_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["color_index_min"] = (double)as_cindex_min.Value;
        }

        private void as_cindex_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["color_index_max"] = (double)as_cindex_max.Value;
        }

        private void as_color_a_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["color_a_min"] = (double)as_color_a_min.Value;
        }

        private void as_color_a_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["color_a_max"] = (double)as_color_a_max.Value;
        }

        private void as_rotate_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["rotate_min"] = (double)as_rotate_min.Value;
        }

        private void as_rotate_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["rotate_max"] = (double)as_rotate_max.Value;
        }

        private void as_spin_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["spin_min"] = (double)as_spin_min.Value;
        }

        private void as_spin_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["spin_max"] = (double)as_spin_max.Value;
        }

        private void as_orbit_min_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["orbit_min"] = (double)as_orbit_min.Value;
        }

        private void as_orbit_max_ValueChanged(object sender, EventArgs e)
        {
            IngestionEngine.Program.AutoScalerDefaults["orbit_max"] = (double)as_orbit_max.Value;
        }


    }
}
