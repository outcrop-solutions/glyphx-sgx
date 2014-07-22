#include "csvreaderwriter.h"
#include <fstream>

namespace SynGlyphX {

    CSVReaderWriter CSVReaderWriter::s_instance;

    CSVReaderWriter::CSVReaderWriter()
    {
        m_predefinedColors[0] = { { 50, 101, 101, 255 } };
        m_predefinedColors[1] = { { 0, 255, 0, 255 } };
        m_predefinedColors[2] = { { 255, 0, 0, 255 } };
        m_predefinedColors[3] = { { 0, 0, 255, 255 } };
        m_predefinedColors[4] = { { 255, 255, 0, 255 } };
        m_predefinedColors[5] = { { 152, 0, 255, 255 } };
        m_predefinedColors[6] = { { 255, 168, 0, 255 } };
        m_predefinedColors[7] = { { 0, 255, 255, 255 } };
        m_predefinedColors[8] = { { 255, 0, 255, 255 } };
        m_predefinedColors[9] = { { 0, 153, 0, 255 } };
        m_predefinedColors[10] = { { 185, 153, 102, 255 } };
        m_predefinedColors[11] = { { 255, 180, 255, 255 } };
        m_predefinedColors[12] = { { 0, 152, 255, 255 } };
        m_predefinedColors[13] = { { 185, 255, 0, 255 } };
        m_predefinedColors[14] = { { 152, 0, 0, 255 } };
        m_predefinedColors[15] = { { 127, 127, 127, 255 } };
        m_predefinedColors[16] = { { 127, 127, 255, 255 } };
        m_predefinedColors[17] = { { 197, 82, 0, 255 } };
        m_predefinedColors[18] = { { 0, 0, 0, 255 } };
        m_predefinedColors[19] = { { 255, 255, 255, 255 } };
    }


    CSVReaderWriter::~CSVReaderWriter()
    {
    }

	void CSVReaderWriter::Write(const std::string& filename, GlyphTree::ConstSharedPtr treeRoot, unsigned long startingId) {

        std::ofstream file;
        try {
            file.open(filename);

            //Write out header, cameras, and grid lines

            file << "id,type,data,selected,parent_id,branch_level,child_id,child_index,child_count,ch_input_id,ch_output_id,ch_last_updated,average,sample,aux_a_x,aux_a_y,aux_a_z,aux_b_x,aux_b_y,aux_b_z,color_shift,rotate_vec_x,rotate_vec_y,rotate_vec_z,rotate_vec_s,scale_x,scale_y,scale_z,translate_x,translate_y,translate_z,tag_offset_x,tag_offset_y,tag_offset_z,rotate_rate_x,rotate_rate_y,rotate_rate_z,rotate_x,rotate_y,rotate_z,scale_rate_x,scale_rate_y,scale_rate_z,translate_rate_x,translate_rate_y,translate_rate_z,translate_vec_x,translate_vec_y,translate_vec_z,shader,geometry,line_width,point_size,ratio,color_index,color_r,color_g,color_b,color_a,color_fade,texture_id,hide,freeze,topo,facet,auto_zoom_x,auto_zoom_y,auto_zoom_z,trigger_hi_x,trigger_hi_y,trigger_hi_z,trigger_lo_x,trigger_lo_y,trigger_lo_z,set_hi_x,set_hi_y,set_hi_z,set_lo_x,set_lo_y,set_lo_z,proximity_x,proximity_y,proximity_z,proximity_mode_x,proximity_mode_y,proximity_mode_z,segments_x,segments_y,segments_z,tag_mode,format_id,table_id,record_id,size" << std::endl;
            file << "1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 50, 101, 101, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 420" << std::endl;
            file << "2, 1, 2, 0, 0, 0, 0, 2, 3, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, -0.143424, -0.826509, -0.544345, 1.000000, 1.000000, 1.000000, 1.344855, 7.749982, 6.275580, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 57.020073, 189.844513, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, -0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 50, 101, 101, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 9.376762, 0.000000, 0.000000, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 420" << std::endl;
            file << "3, 1, 3, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, -1.000000, 1.000000, 1.000000, 1.000000, -0.500000, 0.000000, 571.750000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 50, 101, 101, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 420" << std::endl;
            file << "4, 1, 4, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 1.000000, -0.000000, 1.000000, 1.000000, 1.000000, 0.000000, -90.000000, 7.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 90.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 50, 101, 101, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 420" << std::endl;
            file << "5, 1, 5, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, -1.000000, 0.000000, -0.000000, 1.000000, 1.000000, 1.000000, 85.000000, 0.000000, 7.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 90.000000, 270.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 50, 101, 101, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 0, 16, 16, 0, 0, 0, 0, 0, 420" << std::endl;
            file << "6, 6, 6, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 1.000000, 1.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 1.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 1.000000, 0.000000, 0.100000, 0, 255, 0, 0, 150, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0, 0, 0, 12, 12, 0, 0, 0, 0, 0, 420" << std::endl;

			WriteGlyph(file, *treeRoot->root(), startingId, 0, 0);

            file.close();
        }
        catch (const std::exception& e) {
            file.close();
            throw;
        }
    }

    //This function only handles the one child case.  Need to update
	void CSVReaderWriter::WriteGlyph(std::ofstream& file, const GlyphProperties& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel) {

        unsigned int numberOfChildren = 0;// glyph->GetNumberOfChildren();

        file << id << ", 5, " << id << ", 0, " << parentId << ", " << branchLevel << ", 0, 0, " << numberOfChildren << ", 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, ";
        Vector3 scale = glyph.GetScale();
        Vector3 translate = glyph.GetPosition();
        file << scale[0] << ", " << scale[1] << ", " << scale[2] << ", " << translate[0] << ", " << translate[1] << ", " << translate[2] << ", ";
        Vector3 tagOffset = glyph.GetTagOffset();
        Vector3 rotate = glyph.GetRotation();
        file << tagOffset[0] << ", " << tagOffset[1] << ", " << tagOffset[2] << ", 0, 0, 0, " << rotate[0] << ", " << rotate[1] << ", " << rotate[2] << ", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ";
        Color color = glyph.GetColor();
        file << ConvertGeometryToCSVInt(glyph.GetShape(), glyph.GetSurface()) << ", 1, 0, " << glyph.GetRatio() << ", " << GetColorIndex(color) << ", " << static_cast<int>(color[0]) << ", ";
        file << static_cast<int>(color[1]) << ", " << static_cast<int>(color[2]) << ", " << static_cast<int>(color[3]) << ", 0, 0, 0, 0, " << static_cast<int>(glyph.GetTopology()) << ", ";
        file << "0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 16, 0, 0, 0, 0, " << id << ", 420" << std::endl;
        
        if (numberOfChildren > 0) {
            //WriteGlyph(file, glyph->GetChild(0), id + 1, id, branchLevel + 1);
        }
    }

	unsigned int CSVReaderWriter::ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface) {

		if (shape == GlyphProperties::Shape::Pin) {
			return (2 * shape) + 1 - surface;
		}
		else {
			return (2 * shape) + surface;
		}
    }

    CSVReaderWriter& CSVReaderWriter::GetInstance() {
        return s_instance;
    }

    unsigned short CSVReaderWriter::GetColorIndex(const Color& color) {

        for (unsigned short i = 0; i < MaxPredefinedColors; ++i) {
            if ((color[0] == m_predefinedColors[i][0]) && 
                (color[1] == m_predefinedColors[i][1]) &&
                (color[2] == m_predefinedColors[i][2])) {
                return i;
            }
        }

        //if nothing matches predefined colors return 0
        return 0;
    }

} // namespace SynGlyphX