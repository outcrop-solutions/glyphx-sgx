using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace IngestionEngine
{
    public class Ampz 
    {
        private String _translate_x;  // At L0 -> X translate, At L1+ -> Rotation about Parent
        private String _translate_y;
        private String _translate_z;
        private String _color_index;
        private String _color_r;
        private String _color_g;
        private String _color_b;
        private String _color_a;
        private String _geometry;
        private String _topology;
        private String _record_id;
        private String _translate_rate_x; // At L1+ -> Orbit about Parent
        private String _translate_rate_y; // At l1+ -> Spin  about Self
        private String _scale_x;
        private String _scale_y;
        private String _scale_z;
        private String _ratio;
        private String _color_palette;
        private String _channel;

        public bool isDiff_translate_x;  // At L0 -> X translate, At L1+ -> Rotation about Parent
        public bool isDiff_translate_y;
        public bool isDiff_translate_z;
        public bool isDiff_color_index;
        public bool isDiff_color_r;
        public bool isDiff_color_g;
        public bool isDiff_color_b;
        public bool isDiff_color_a;
        public bool isDiff_geometry;
        public bool isDiff_topology;
        public bool isDiff_record_id;
        public bool isDiff_translate_rate_x; // At L1+ -> Orbit about Parent
        public bool isDiff_translate_rate_y; // At l1+ -> Spin  about Self
        public bool isDiff_scale_x;
        public bool isDiff_scale_y;
        public bool isDiff_scale_z;
        public bool isDiff_ratio;
        public bool isDiff_color_palette;
        public bool isDiff_channel;

        const int Depth = 16;
        const int Level = 16; // Reset to 32 after debugs
        public string translate_x { get { return _translate_x; } set { _translate_x = value; } }
        public string translate_y { get { return _translate_y; } set { _translate_y = value; } }
        public string translate_z { get { return _translate_z; } set { _translate_z = value; } }
        public string color_index { get { return _color_index; } set { _color_index = value; } }
        public string color_r { get { return _color_r; } set { _color_r = value; } }
        public string color_g { get { return _color_g; } set { _color_g = value; } }
        public string color_b { get { return _color_b; } set { _color_b = value; } }
        public string color_a { get { return _color_a; } set { _color_a = value; } }
        public string geometry { get { return _geometry; } set { _geometry = value; } }
        public string topology { get { return _topology; } set { _topology = value; } }
        public string record_id { get { return _record_id; } set { _record_id = value; } }
        public string translate_rate_x { get { return _translate_rate_x; } set { _translate_rate_x = value; } }
        public string translate_rate_y { get { return _translate_rate_y; } set { _translate_rate_y = value; } }
        public string scale_x { get { return _scale_x; } set { _scale_x = value; } }
        public string scale_y { get { return _scale_y; } set { _scale_y = value; } }
        public string scale_z { get { return _scale_z; } set { _scale_z = value; } }
        public string ratio { get { return _ratio; } set { _ratio = value; } }
        public string color_palette { get { return _color_palette; } set { _color_palette = value; } }
        public string channel { get { return _channel; } set { _channel = value; } }
        
    }



}
