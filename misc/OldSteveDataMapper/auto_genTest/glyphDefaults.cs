using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IngestionEngine
{
    class glyphDefaults
    {
        private String _translate_x_min;  // At L0 -> X translate, At L1+ -> Rotation about Parent
        private String _translate_y_min;
        private String _translate_z_min;
        private String _color_index_min;
        private String _color_r_min;
        private String _color_g_min;
        private String _color_b_min;
        private String _color_a_min;
        private String _geometry_min;
        private String _topology_min;
        private String _record_id_min;
        private String _translate_rate_x_min; // At L1+ -> Orbit about Parent
        private String _translate_rate_y_min; // At l1+ -> Spin  about Self
        private String _scale_x_min;
        private String _scale_y_min;
        private String _scale_z_min;
        private String _ratio_min; 
        
        private String _translate_x_max;  // At L0 -> X translate, At L1+ -> Rotation about Parent
        private String _translate_y_max;
        private String _translate_z_max;
        private String _color_index_max;
        private String _color_r_max;
        private String _color_g_max;
        private String _color_b_max;
        private String _color_a_max;
        private String _geometry_max;
        private String _topology_max;
        private String _record_id_max;
        private String _translate_rate_x_max; // At L1+ -> Orbit about Parent
        private String _translate_rate_y_max; // At l1+ -> Spin  about Self
        private String _scale_x_max;
        private String _scale_y_max;
        private String _scale_z_max;
        private String _ratio_max;

        public string translate_x_max { get { return _translate_x_max; } set { _translate_x_max = value; } }
        public string translate_y_max { get { return _translate_y_max; } set { _translate_y_max = value; } }
        public string translate_z_max { get { return _translate_z_max; } set { _translate_z_max = value; } }
        public string color_index_max { get { return _color_index_max; } set { _color_index_max = value; } }
        public string color_r_max { get { return _color_r_max; } set { _color_r_max = value; } }
        public string color_g_max { get { return _color_g_max; } set { _color_g_max = value; } }
        public string color_b_max { get { return _color_b_max; } set { _color_b_max = value; } }
        public string color_a_max { get { return _color_a_max; } set { _color_a_max = value; } }
        public string geometry_max { get { return _geometry_max; } set { _geometry_max = value; } }
        public string topology_max { get { return _topology_max; } set { _topology_max = value; } }
        public string record_id_max { get { return _record_id_max; } set { _record_id_max = value; } }
        public string translate_rate_x_max { get { return _translate_rate_x_max; } set { _translate_rate_x_max = value; } }
        public string translate_rate_y_max { get { return _translate_rate_y_max; } set { _translate_rate_y_max = value; } }
        public string scale_x_max { get { return _scale_x_max; } set { _scale_x_max = value; } }
        public string scale_y_max { get { return _scale_y_max; } set { _scale_y_max = value; } }
        public string scale_z_max { get { return _scale_z_max; } set { _scale_z_max = value; } }
        public string ratio_max { get { return _ratio_max; } set { _ratio_max = value; } }

        public string translate_x_min { get { return _translate_x_min; } set { _translate_x_min = value; } }
        public string translate_y_min { get { return _translate_y_min; } set { _translate_y_min = value; } }
        public string translate_z_min { get { return _translate_z_min; } set { _translate_z_min = value; } }
        public string color_index_min { get { return _color_index_min; } set { _color_index_min = value; } }
        public string color_r_min { get { return _color_r_min; } set { _color_r_min = value; } }
        public string color_g_min { get { return _color_g_min; } set { _color_g_min = value; } }
        public string color_b_min { get { return _color_b_min; } set { _color_b_min = value; } }
        public string color_a_min { get { return _color_a_min; } set { _color_a_min = value; } }
        public string geometry_min { get { return _geometry_min; } set { _geometry_min = value; } }
        public string topology_min { get { return _topology_min; } set { _topology_min = value; } }
        public string record_id_min { get { return _record_id_min; } set { _record_id_min = value; } }
        public string translate_rate_x_min { get { return _translate_rate_x_min; } set { _translate_rate_x_min = value; } }
        public string translate_rate_y_min { get { return _translate_rate_y_min; } set { _translate_rate_y_min = value; } }
        public string scale_x_min { get { return _scale_x_min; } set { _scale_x_min = value; } }
        public string scale_y_min { get { return _scale_y_min; } set { _scale_y_min = value; } }
        public string scale_z_min { get { return _scale_z_min; } set { _scale_z_min = value; } }
        public string ratio_min { get { return _ratio_min; } set { _ratio_min = value; } }

    }
}
