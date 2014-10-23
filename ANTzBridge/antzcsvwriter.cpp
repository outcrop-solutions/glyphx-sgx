#include "antzcsvwriter.h"
#include <fstream>
#include <locale>

namespace SynGlyphX {

	ANTzCSVWriter ANTzCSVWriter::s_instance;

	ANTzCSVWriter::ANTzCSVWriter() :
		m_nodeHeaders({ "id", "type", "data", "selected", "parent_id", "branch_level", "child_id", "child_index", "child_count", "ch_input_id", "ch_output_id", "ch_last_updated", "average", "sample", "aux_a_x", "aux_a_y", "aux_a_z", "aux_b_x", "aux_b_y", "aux_b_z", "color_shift", "rotate_vec_x", "rotate_vec_y", "rotate_vec_z", "rotate_vec_s", "scale_x", "scale_y", "scale_z", "translate_x", "translate_y", "translate_z", "tag_offset_x", "tag_offset_y", "tag_offset_z", "rotate_rate_x", "rotate_rate_y", "rotate_rate_z", "rotate_x", "rotate_y", "rotate_z", "scale_rate_x", "scale_rate_y", "scale_rate_z", "translate_rate_x", "translate_rate_y", "translate_rate_z", "translate_vec_x", "translate_vec_y", "translate_vec_z", "shader", "geometry", "line_width", "point_size", "ratio", "color_index", "color_r", "color_g", "color_b", "color_a", "color_fade", "texture_id", "hide", "freeze", "topo", "facet", "auto_zoom_x", "auto_zoom_y", "auto_zoom_z", "trigger_hi_x", "trigger_hi_y", "trigger_hi_z", "trigger_lo_x", "trigger_lo_y", "trigger_lo_z", "set_hi_x", "set_hi_y", "set_hi_z", "set_lo_x", "set_lo_y", "set_lo_z", "proximity_x", "proximity_y", "proximity_z", "proximity_mode_x", "proximity_mode_y", "proximity_mode_z", "segments_x", "segments_y", "segments_z", "tag_mode", "format_id", "table_id", "record_id", "size" }),
		m_tagHeaders({ "id", "record_id", "table_id", "title", "description" }),
		m_channelHeaders({ "cycleCount", "ch1", "ch2", "ch3", "ch4", "ch5", "ch6", "ch7", "ch8", "ch9", "ch10", "ch11", "ch12", "ch13", "ch14", "ch15", "ch16", "ch17", "ch18", "ch19", "ch20", "ch21", "ch22", "ch23", "ch24", "ch25", "ch26", "ch27", "ch28", "ch29", "ch30", "ch31", "ch32", "ch33", "ch34", "ch35", "ch36", "ch37", "ch38", "ch39", "ch40", "ch41", "ch42", "ch43", "ch44", "ch45", "ch46", "ch47", "ch48", "ch49", "ch50", "ch51", "ch52", "ch53", "ch54", "ch55", "ch56", "ch57", "ch58", "ch59", "ch60", "ch61", "ch62", "ch63", "ch64", "ch65", "ch66", "ch67", "ch68", "ch69", "ch70", "ch71", "ch72", "ch73", "ch74", "ch75", "ch76", "ch77", "ch78", "ch79", "ch80", "ch81", "ch82", "ch83", "ch84", "ch85", "ch86", "ch87", "ch88", "ch89", "ch90", "ch91", "ch92", "ch93", "ch94", "ch95", "ch96", "ch97", "ch98", "ch99", "ch100" }),
		m_channelMapHeaders({ "id", "channel_id", "track_id", "attribute", "track_table_id", "ch_map_table_id", "record_id" })
	{
		m_predefinedColors[0] = SynGlyphX::Color({ { 50, 101, 101, 255 } });
		m_predefinedColors[1] = SynGlyphX::Color({ { 0, 255, 0, 255 } });
		m_predefinedColors[2] = SynGlyphX::Color({ { 255, 0, 0, 255 } });
		m_predefinedColors[3] = SynGlyphX::Color({ { 0, 0, 255, 255 } });
		m_predefinedColors[4] = SynGlyphX::Color({ { 255, 255, 0, 255 } });
		m_predefinedColors[5] = SynGlyphX::Color({ { 152, 0, 255, 255 } });
		m_predefinedColors[6] = SynGlyphX::Color({ { 255, 168, 0, 255 } });
		m_predefinedColors[7] = SynGlyphX::Color({ { 0, 255, 255, 255 } });
		m_predefinedColors[8] = SynGlyphX::Color({ { 255, 0, 255, 255 } });
		m_predefinedColors[9] = SynGlyphX::Color({ { 0, 153, 0, 255 } });
		m_predefinedColors[10] = SynGlyphX::Color({ { 185, 153, 102, 255 } });
		m_predefinedColors[11] = SynGlyphX::Color({ { 255, 180, 255, 255 } });
		m_predefinedColors[12] = SynGlyphX::Color({ { 0, 152, 255, 255 } });
		m_predefinedColors[13] = SynGlyphX::Color({ { 185, 255, 0, 255 } });
		m_predefinedColors[14] = SynGlyphX::Color({ { 152, 0, 0, 255 } });
		m_predefinedColors[15] = SynGlyphX::Color({ { 127, 127, 127, 255 } });
		m_predefinedColors[16] = SynGlyphX::Color({ { 127, 127, 255, 255 } });
		m_predefinedColors[17] = SynGlyphX::Color({ { 197, 82, 0, 255 } });
		m_predefinedColors[18] = SynGlyphX::Color({ { 0, 0, 0, 255 } });
		m_predefinedColors[19] = SynGlyphX::Color({ { 255, 255, 255, 255 } });
	}


	ANTzCSVWriter::~ANTzCSVWriter()
	{
	}

	void ANTzCSVWriter::Write(const std::string& filename, const std::string& tagFilename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId) {

		std::ofstream file;
		std::ofstream tagfile;
		try {

			unsigned long id = startingId;

			file.open(filename);

			//Write out header, cameras, and grid lines
			file << CreateString(m_nodeHeaders) << std::endl;
			file << "1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.000000,1.000000,0.000000,0.000000,0.000000,1.000000,1.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,0,16,16,0,0,0,0,0,420" << std::endl;
			file << "2,1,2,0,0,0,0,2,3,0,0,0,0,1,0,0,0,0,0,0,0.000000,0.000000,-0.143424,-0.826509,-0.544345,1.000000,1.000000,1.000000,1.344855,7.749982,6.275580,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,57.020073,189.844513,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,-0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,9.376762,0.000000,0.000000,0,0,0,16,16,0,0,0,0,0,420" << std::endl;
			file << "3,1,3,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,-1.000000,1.000000,1.000000,1.000000,-0.500000,0.000000,571.750000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,0,16,16,0,0,0,0,0,420" << std::endl;
			file << "4,1,4,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.000000,0.000000,0.000000,1.000000,-0.000000,1.000000,1.000000,1.000000,0.000000,-90.000000,7.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,90.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,0,16,16,0,0,0,0,0,420" << std::endl;
			file << "5,1,5,0,2,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.000000,0.000000,-1.000000,0.000000,-0.000000,1.000000,1.000000,1.000000,85.000000,0.000000,7.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,90.000000,270.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,50,101,101,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,0,16,16,0,0,0,0,0,420" << std::endl;
			
			file << "6,6,6,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0.000000,1.000000,0.000000,0.000000,0.000000,1.000000,1.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,1.000000,0.000000,0.100000,0,255,0,0,150,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0,0,0,12,6,0,0,0,0,0,420" << std::endl;

			for (const GlyphTree::ConstSharedPtr& tree : trees) {
				id = WriteGlyph(file, tree, tree->root(), id, 0, 0);
			}

			file.close();

			tagfile.open(tagFilename);

			m_numTagsWritten = 0;
			unsigned long tagID = startingId;
			tagfile << CreateString(m_tagHeaders) << std::endl;
			for (const GlyphTree::ConstSharedPtr& tree : trees) {
				tagID = WriteGlyphTag(tagfile, tree, tree->root(), tagID);
			}

			tagfile.close();
		}
		catch (const std::exception& e) {

			file.close();
			tagfile.close();
			throw;
		}
	}

	unsigned long ANTzCSVWriter::WriteGlyphTag(std::ofstream& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id) {

		unsigned int numberOfChildren = tree->children(glyph);

		std::string tag;
		if (glyph->GetTag().empty()) {

			tag = "No Tag";
		}
		else {

			tag.assign(glyph->GetTag().begin(), glyph->GetTag().end());
		}

		file << m_numTagsWritten++ << "," << id << ",0,\"" << tag << "\",\" \"" << std::endl;

		unsigned long childId = id + 1;
		for (unsigned int i = 0; i < numberOfChildren; ++i) {
			childId = WriteGlyphTag(file, tree, tree->child(glyph, i), childId);
		}

		return childId;
	}

	unsigned long ANTzCSVWriter::WriteGlyph(std::ofstream& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel) {

		unsigned int numberOfChildren = tree->children(glyph);

		file << id << ",5," << id << ",0," << parentId << "," << branchLevel << ",0,0," << numberOfChildren << ",0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,";
		Vector3 scale = glyph->GetScale();
		Vector3 translate = glyph->GetPosition();
		file << scale[0] << "," << scale[1] << "," << scale[2] << "," << translate[0] << "," << translate[1] << "," << translate[2] << ",";
		Vector3 tagOffset = glyph->GetTagOffset();
		Vector3 rotate = glyph->GetRotation();
		file << tagOffset[0] << "," << tagOffset[1] << "," << tagOffset[2] << ",0,0,0," << rotate[0] << "," << rotate[1] << "," << rotate[2] << ",0,0,0,0,0,0,0,0,0,0,";
		Color color = glyph->GetColor();
		file << ConvertGeometryToCSVInt(glyph->GetShape(), glyph->GetSurface()) << ",1,0," << glyph->GetRatio() << "," << GetColorIndex(color) << "," << static_cast<int>(color[0]) << ",";
		file << static_cast<int>(color[1]) << "," << static_cast<int>(color[2]) << "," << static_cast<int>(color[3]) << ",0,0,0,0," << static_cast<int>(glyph->GetTopology()) << ",";
		file << "0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,16,16,0,0,0,0," << id << ",420" << std::endl;

		unsigned long childId = id + 1;
		for (unsigned int i = 0; i < numberOfChildren; ++i) {
			childId = WriteGlyph(file, tree, tree->child(glyph, i), childId, id, branchLevel + 1);
		}

		return childId;
	}

	unsigned int ANTzCSVWriter::ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface) {

		if (shape == GlyphProperties::Shape::Pin) {
			return (2 * shape) + 1 - surface;
		}
		else {
			return (2 * shape) + surface;
		}
	}

	ANTzCSVWriter& ANTzCSVWriter::GetInstance() {
		return s_instance;
	}

	unsigned short ANTzCSVWriter::GetColorIndex(const Color& color) {

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

	std::string ANTzCSVWriter::CreateString(const CSVFileReader::CSVValues& values) {

		if (values.empty()) {

			throw std::invalid_argument("CSVValues object has 0 values");
		}

		std::string valuesString = values[0];
		for (unsigned int i = 1; i < values.size(); ++i) {

			valuesString += "," + values[i];
		}

		return valuesString;
	}

	const CSVFileReader::CSVValues& ANTzCSVWriter::GetNodeHeaders() const {

		return m_nodeHeaders;
	}

	const CSVFileReader::CSVValues& ANTzCSVWriter::GetTagHeaders() const {

		return m_tagHeaders;
	}

	const CSVFileReader::CSVValues& ANTzCSVWriter::GetChannelHeaders() const {

		return m_channelHeaders;
	}

	const CSVFileReader::CSVValues& ANTzCSVWriter::GetChannelMapHeaders() const {

		return m_channelMapHeaders;
	}

} // namespace SynGlyphX