using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CsvHelper;
using CsvHelper.Configuration;
using System.ComponentModel;

namespace IngestionEngine
{
    class AntzCollection : IEnumerable
    {
        private List<sgxAntz> noders { get; set; }
        

        public IEnumerator GetEnumerator()
        {
            //return noders.GetEnumerator();
            foreach (object n in noders)
            {
                // Lets check for end of list (its bad code since we used arrays)
                if (n == null)
                {
                    break;
                }

                // Return the current element and then on next function call 
                // resume from next element rather than starting all over again;
                yield return n;
            }
        }

    }
}

namespace IngestionEngine
{
    public class sgxAntz 
    {
         
        public Int32 id { get; set; }
        public Int32 type { get; set; }  
        public Int32 data { get; set; }  
        public Int32 selected { get; set; }  
        public Int32 parent_id { get; set; }  
        public Int32 branch_level { get; set; }  
        public Int32 child_id { get; set; }  
        public Int32 child_index { get; set; }  
        public Int32 child_count { get; set; }  
        public Int32 ch_input_id { get; set; }  
        public Int32 ch_output_id { get; set; }  
        public Int32 ch_last_updated { get; set; }  
        public Int32 average { get; set; }  
        public Int32 sample { get; set; }  
        public Int32 aux_a_x { get; set; }  
        public Int32 aux_a_y { get; set; }  
        public Int32 aux_a_z { get; set; }  
        public Int32 aux_b_x { get; set; }  
        public Int32 aux_b_y { get; set; }  
        public Int32 aux_b_z { get; set; }  
        public float color_shift { get; set; }  
        public float rotate_vec_x { get; set; }  
        public float rotate_vec_y { get; set; }  
        public float rotate_vec_z { get; set; }  
        public float rotate_vec_s { get; set; }  
        public float scale_x { get; set; }  
        public float scale_y { get; set; }  
        public float scale_z { get; set; }  
        public float translate_x { get; set; }  
        public float translate_y { get; set; }  
        public float translate_z { get; set; }  
        public float tag_offset_x { get; set; }  
        public float tag_offset_y { get; set; }  
        public float tag_offset_z { get; set; }  
        public float rotate_rate_x { get; set; } 
        public float rotate_rate_y { get; set; }  
        public float rotate_rate_z { get; set; }  
        public float rotate_x { get; set; }  
        public float rotate_y { get; set; }  
        public float rotate_z { get; set; }  
        public float scale_rate_x { get; set; }  
        public float scale_rate_y { get; set; }  
        public float scale_rate_z { get; set; }  
        public float translate_rate_x { get; set; }  
        public float translate_rate_y { get; set; }  
        public float translate_rate_z { get; set; }  
        public float translate_vec_x { get; set; }  
        public float translate_vec_y { get; set; }  
        public float translate_vec_z { get; set; }  
        public Int32 shader { get; set; }  
        public Int32 geometry { get; set; }  
        public float line_width { get; set; }  
        public float point_size { get; set; }  
        public float ratio { get; set; }  
        public Int32 color_index { get; set; } 
        public Int32 color_r { get; set; }  
        public Int32 color_g { get; set; }  
        public Int32 color_b { get; set; }  
        public Int32 color_a { get; set; }  
        public Int32 color_fade { get; set; }  
        public Int32 texture_id { get; set; }  
        public Int32 hide { get; set; }  
        public Int32 freeze { get; set; }  
        public Int32 topo { get; set; }  
        public Int32 facet { get; set; }  
        public Int32 auto_zoom_x { get; set; }  
        public Int32 auto_zoom_y { get; set; }  
        public Int32 auto_zoom_z { get; set; }  
        public Int32 trigger_hi_x { get; set; }  
        public Int32 trigger_hi_y { get; set; }  
        public Int32 trigger_hi_z { get; set; }  
        public Int32 trigger_lo_x { get; set; }  
        public Int32 trigger_lo_y { get; set; }  
        public Int32 trigger_lo_z { get; set; }  
        public float set_hi_x { get; set; }  
        public float set_hi_y { get; set; }  
        public float set_hi_z { get; set; }  
        public float set_lo_x { get; set; }  
        public float set_lo_y { get; set; }  
        public float set_lo_z { get; set; }  
        public float proximity_x { get; set; }  
        public float proximity_y { get; set; }  
        public float proximity_z { get; set; }  
        public Int32 proximity_mode_x { get; set; }  
        public Int32 proximity_mode_y { get; set; }  
        public Int32 proximity_mode_z { get; set; }  
        public Int32 segments_x { get; set; }  
        public Int32 segments_y { get; set; }  
        public Int32 segments_z { get; set; }  
        public Int32 tag_mode { get; set; }  
        public Int32 format_id { get; set; }  
        public Int32 table_id { get; set; }  
        public Int32 record_id { get; set; }  
        public Int32 size { get; set; }  

    }


    class sgxAntzMap : CsvClassMap<sgxAntz>
    {
        public override void CreateMap()
        {

            Map(m => m.id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            
            
            
            Map(m => m.type).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("type").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.data).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("data").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.selected).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("selected").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.parent_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("parent_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.branch_level).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("branch_level").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.child_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("child_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.child_index).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("child_index").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.child_count).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("child_count").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.ch_input_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("ch_input_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.ch_output_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("ch_output_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.ch_last_updated).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("ch_last_updated").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.average).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("average").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.sample).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("sample").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_a_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_a_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_a_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_a_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_a_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_a_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_b_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_b_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_b_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_b_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.aux_b_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("aux_b_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_shift).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("color_shift").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();//float
            Map(m => m.rotate_vec_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_vec_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>(); 
            Map(m => m.rotate_vec_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_vec_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_vec_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_vec_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_vec_s).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_vec_s").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.tag_offset_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("tag_offset_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.tag_offset_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("tag_offset_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.tag_offset_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("tag_offset_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_rate_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_rate_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_rate_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_rate_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_rate_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_rate_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.rotate_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("rotate_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_rate_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_rate_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_rate_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_rate_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.scale_rate_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("scale_rate_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_rate_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_rate_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_rate_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_rate_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_rate_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_rate_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_vec_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_vec_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_vec_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_vec_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.translate_vec_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("translate_vec_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.shader).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("shader").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();  //int
            Map(m => m.geometry).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("geometry").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.line_width).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("line_width").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.point_size).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("point_size").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.ratio).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("ratio").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();  //float
            Map(m => m.color_index).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_index").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_r).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_r").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_g).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_g").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_b).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_b").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_a).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_a").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.color_fade).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("color_fade").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.texture_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("texture_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.hide).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("hide").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.freeze).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("freeze").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.topo).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("topo").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.facet).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("facet").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.auto_zoom_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("auto_zoom_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.auto_zoom_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("auto_zoom_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.auto_zoom_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("auto_zoom_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_hi_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_hi_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_hi_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_hi_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_hi_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_hi_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_lo_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_lo_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_lo_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_lo_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.trigger_lo_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("trigger_lo_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.set_hi_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_hi_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();  //float
            Map(m => m.set_hi_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_hi_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.set_hi_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_hi_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.set_lo_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_lo_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.set_lo_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_lo_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.set_lo_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("set_lo_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.proximity_x).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("proximity_x").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.proximity_y).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("proximity_y").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.proximity_z).TypeConverterOption(System.Globalization.NumberStyles.Number).Name("proximity_z").TypeConverter<CsvHelper.TypeConversion.DecimalConverter>();
            Map(m => m.proximity_mode_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("proximity_mode_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>(); //int
            Map(m => m.proximity_mode_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("proximity_mode_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.proximity_mode_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("proximity_mode_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.segments_x).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("segments_x").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.segments_y).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("segments_y").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.segments_z).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("segments_z").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.tag_mode).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("tag_mode").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.format_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("format_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.table_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("table_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.record_id).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("record_id").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
            Map(m => m.size).TypeConverterOption(System.Globalization.NumberStyles.Integer).Name("size").TypeConverter<CsvHelper.TypeConversion.Int32Converter>();
        
        }

    }
}
