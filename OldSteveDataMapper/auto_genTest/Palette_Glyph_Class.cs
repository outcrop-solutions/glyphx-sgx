using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Threading.Tasks;

namespace IngestionEngine
{
    class Palette_Glyph_Class
    {
        private Color _lowColor;
        private Color _midColor;
        private Color _hiColor;
        private Int32 _lowNumber;
        private Int32 _midNumber;
        private Int32 _hiNumber;
        private Int32 _gradType;
        private String _name;

        public Color lowColor { get { return _lowColor; } set { _lowColor = value; } }
        public Color midColor { get { return _midColor; } set { _midColor = value; } }
        public Color hiColor { get { return _hiColor; } set { _hiColor = value; } }
        public Int32 lowNumber { get { return _lowNumber; } set { _lowNumber = value; } }
        public Int32 midNumber { get { return _midNumber; } set { _midNumber = value; } }
        public Int32 hiNumber { get { return _hiNumber; } set { _hiNumber = value; } }
        public Int32 gradType { get { return _gradType; } set { _gradType = value; } }
        public String name { get { return _name; } set { _name = value; } }

        public Palette_Glyph_Class()
        {
            lowColor    = Color.Empty;
            midColor    = Color.Empty;
            hiColor     = Color.Empty;
            lowNumber   = 0;
            midNumber   = 128;
            hiNumber    = 255;
            name        = "FiftyShadesofGray";
        }
    }

    public interface List<String, Palette_Glyph_Class>
    {
    }
}
