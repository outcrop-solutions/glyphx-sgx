#include "antzcsvwriter.h"
#include <fstream>
#include <locale>
#include <boost/lexical_cast.hpp>
#include "glyphnodeconverter.h"

namespace SynGlyphXANTz {

	ANTzCSVWriter ANTzCSVWriter::s_instance;

	ANTzCSVWriter::ANTzCSVWriter() :
		m_nodeHeaders({ L"id", L"type", L"data", L"selected", L"parent_id", L"branch_level", L"child_id", L"child_index", L"child_count", L"ch_input_id", L"ch_output_id", L"ch_last_updated", L"average", L"sample", L"aux_a_x", L"aux_a_y", L"aux_a_z", L"aux_b_x", L"aux_b_y", L"aux_b_z", L"color_shift", L"rotate_vec_x", L"rotate_vec_y", L"rotate_vec_z", L"rotate_vec_s", L"scale_x", L"scale_y", L"scale_z", L"translate_x", L"translate_y", L"translate_z", L"tag_offset_x", L"tag_offset_y", L"tag_offset_z", L"rotate_rate_x", L"rotate_rate_y", L"rotate_rate_z", L"rotate_x", L"rotate_y", L"rotate_z", L"scale_rate_x", L"scale_rate_y", L"scale_rate_z", L"translate_rate_x", L"translate_rate_y", L"translate_rate_z", L"translate_vec_x", L"translate_vec_y", L"translate_vec_z", L"shader", L"geometry", L"line_width", L"point_size", L"ratio", L"color_index", L"color_r", L"color_g", L"color_b", L"color_a", L"color_fade", L"texture_id", L"hide", L"freeze", L"topo", L"facet", L"auto_zoom_x", L"auto_zoom_y", L"auto_zoom_z", L"trigger_hi_x", L"trigger_hi_y", L"trigger_hi_z", L"trigger_lo_x", L"trigger_lo_y", L"trigger_lo_z", L"set_hi_x", L"set_hi_y", L"set_hi_z", L"set_lo_x", L"set_lo_y", L"set_lo_z", L"proximity_x", L"proximity_y", L"proximity_z", L"proximity_mode_x", L"proximity_mode_y", L"proximity_mode_z", L"segments_x", L"segments_y", L"segments_z", L"tag_mode", L"format_id", L"table_id", L"record_id", L"size" }),
		m_tagHeaders({ L"id", L"record_id", L"table_id", L"title", L"description" }),
		m_channelHeaders({ L"cycleCount", L"ch1", L"ch2", L"ch3", L"ch4", L"ch5", L"ch6", L"ch7", L"ch8", L"ch9", L"ch10", L"ch11", L"ch12", L"ch13", L"ch14", L"ch15", L"ch16", L"ch17", L"ch18", L"ch19", L"ch20", L"ch21", L"ch22", L"ch23", L"ch24", L"ch25", L"ch26", L"ch27", L"ch28", L"ch29", L"ch30", L"ch31", L"ch32", L"ch33", L"ch34", L"ch35", L"ch36", L"ch37", L"ch38", L"ch39", L"ch40", L"ch41", L"ch42", L"ch43", L"ch44", L"ch45", L"ch46", L"ch47", L"ch48", L"ch49", L"ch50", L"ch51", L"ch52", L"ch53", L"ch54", L"ch55", L"ch56", L"ch57", L"ch58", L"ch59", L"ch60", L"ch61", L"ch62", L"ch63", L"ch64", L"ch65", L"ch66", L"ch67", L"ch68", L"ch69", L"ch70", L"ch71", L"ch72", L"ch73", L"ch74", L"ch75", L"ch76", L"ch77", L"ch78", L"ch79", L"ch80", L"ch81", L"ch82", L"ch83", L"ch84", L"ch85", L"ch86", L"ch87", L"ch88", L"ch89", L"ch90", L"ch91", L"ch92", L"ch93", L"ch94", L"ch95", L"ch96", L"ch97", L"ch98", L"ch99", L"ch100" }),
		m_channelMapHeaders({ L"id", L"channel_id", L"track_id", L"attribute", L"track_table_id", L"ch_map_table_id", L"record_id" }),
		m_noURLLocation(L"nourl.html")
	{
		m_predefinedColors[0] = SynGlyphX::GlyphColor({ { 50, 101, 101 } });
		m_predefinedColors[1] = SynGlyphX::GlyphColor({ { 0, 255, 0 } });
		m_predefinedColors[2] = SynGlyphX::GlyphColor({ { 255, 0, 0 } });
		m_predefinedColors[3] = SynGlyphX::GlyphColor({ { 0, 0, 255 } });
		m_predefinedColors[4] = SynGlyphX::GlyphColor({ { 255, 255, 0 } });
		m_predefinedColors[5] = SynGlyphX::GlyphColor({ { 152, 0, 255 } });
		m_predefinedColors[6] = SynGlyphX::GlyphColor({ { 255, 168, 0 } });
		m_predefinedColors[7] = SynGlyphX::GlyphColor({ { 0, 255, 255 } });
		m_predefinedColors[8] = SynGlyphX::GlyphColor({ { 255, 0, 255 } });
		m_predefinedColors[9] = SynGlyphX::GlyphColor({ { 0, 153, 0 } });
		m_predefinedColors[10] = SynGlyphX::GlyphColor({ { 185, 153, 102 } });
		m_predefinedColors[11] = SynGlyphX::GlyphColor({ { 255, 180, 255 } });
		m_predefinedColors[12] = SynGlyphX::GlyphColor({ { 0, 152, 255 } });
		m_predefinedColors[13] = SynGlyphX::GlyphColor({ { 185, 255, 0 } });
		m_predefinedColors[14] = SynGlyphX::GlyphColor({ { 152, 0, 0 } });
		m_predefinedColors[15] = SynGlyphX::GlyphColor({ { 127, 127, 127 } });
		m_predefinedColors[16] = SynGlyphX::GlyphColor({ { 127, 127, 255 } });
		m_predefinedColors[17] = SynGlyphX::GlyphColor({ { 197, 82, 0 } });
		m_predefinedColors[18] = SynGlyphX::GlyphColor({ { 0, 0, 0 } });
		m_predefinedColors[19] = SynGlyphX::GlyphColor({ { 255, 255, 255 } });

		m_cameras[0] = { L"1", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"1.000000", L"1.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"1.000000", L"0.000000", L"0.100000", L"0", L"50", L"101", L"101", L"255", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0", L"0", L"420" };
		m_cameras[1] = { L"2", L"1", L"2", L"0", L"0", L"0", L"0", L"2", L"3", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"1.000000", L"1.000000", L"1.000000", L"0.000000", L"-350.000000", L"350.000000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"45.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"1.000000", L"0.000000", L"0.100000", L"0", L"50", L"101", L"101", L"255", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"488.000000", L"0.000000", L"0.000000", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0", L"0", L"420" };
		m_cameras[2] = { L"3", L"1", L"3", L"0", L"2", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"-1.000000", L"1.000000", L"1.000000", L"1.000000", L"-0.500000", L"0.000000", L"571.750000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"1.000000", L"0.000000", L"0.100000", L"0", L"50", L"101", L"101", L"255", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0", L"0", L"420" };
		m_cameras[3] = { L"4", L"1", L"4", L"0", L"2", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"1.000000", L"-0.000000", L"1.000000", L"1.000000", L"1.000000", L"0.000000", L"-90.000000", L"7.000000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"90.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"1.000000", L"0.000000", L"0.100000", L"0", L"50", L"101", L"101", L"255", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0", L"0", L"420" };
		m_cameras[4] = { L"5", L"1", L"5", L"0", L"2", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"-1.000000", L"0.000000", L"-0.000000", L"1.000000", L"1.000000", L"1.000000", L"85.000000", L"0.000000", L"7.000000", L"0.000000", L"0.000000", L"1.000000", L"0.000000", L"0.000000", L"0.000000", L"90.000000", L"270.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"1.000000", L"0.000000", L"0.100000", L"0", L"50", L"101", L"101", L"255", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0.000000", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0", L"0", L"420" };
	}


	ANTzCSVWriter::~ANTzCSVWriter()
	{
	}

	void ANTzCSVWriter::SetNOURLLocation(const std::wstring& noURLLocation) {

		m_noURLLocation = noURLLocation;
	}

	void ANTzCSVWriter::Write(const FilenameList& filenames, SynGlyphX::GlyphGraph::ConstSharedVector& trees, const std::vector<ANTzGrid>& grids, OutputPlatform platform) {

		if (filenames[s_nodeFilenameIndex].empty()) {

			throw std::invalid_argument("Node filename is empty.");
		}

		try {

			SynGlyphX::CSVFileWriter nodeFile(filenames[s_nodeFilenameIndex]);
			SynGlyphX::CSVFileWriter tagFile; 
			std::wofstream redirectFile;
			
			if (!filenames[s_tagFilenameIndex].empty()) {

				tagFile.Open(filenames[s_tagFilenameIndex]);
			}

			if ((!filenames[s_redirectFilenameIndex].empty()) && (platform == OutputPlatform::WindowsZSpace)) {

				redirectFile.open(filenames[s_redirectFilenameIndex]);
				WriteRedirectHeader(redirectFile, platform);
			}

			m_numTagsWritten = 0;
			unsigned long startingId = WriteHeaders(nodeFile, tagFile, grids);

			for (const SynGlyphX::GlyphGraph::ConstSharedPtr& tree : trees) {

				m_labelToANTzIDMap.clear();
				m_labelToANTzBranchLevelMap.clear();

				unsigned long id = WriteGlyphAndChildren(nodeFile, tree, tree->GetRoot(), startingId, 0, 0, false);

				for (auto& link : tree->GetLinks()) {

					unsigned int branchLevel = std::max(m_labelToANTzBranchLevelMap[link.first.first], m_labelToANTzBranchLevelMap[link.first.second]) + 1;
					id = WriteGlyph(nodeFile, link.second, 0, id, m_labelToANTzIDMap[link.first.first], m_labelToANTzIDMap[link.first.second], branchLevel, true);
				}

				if (tagFile.IsOpen()) {

					WriteGlyphTag(tagFile, tree, tree->GetRoot(), (platform != OutputPlatform::WindowsZSpace));
				}

				if (redirectFile.is_open()) {

					WriteURLRedirects(redirectFile, tree, tree->GetRoot(), platform);
				}

				startingId = id;
			}

			if (redirectFile.is_open()) {

				redirectFile << L"start \"\" \"" << m_noURLLocation << "\"\nexit\n";
			}

			nodeFile.Close();
			tagFile.Close();
			redirectFile.close();
		}
		catch (const std::exception& e) {

			throw;
		}
 	}

	void ANTzCSVWriter::WriteRedirectHeader(std::wofstream& file, OutputPlatform platform) {

		if (platform == OutputPlatform::WindowsZSpace) {

			file << L"@ECHO OFF\n\n";
		}
		else {


		}
	}

	void ANTzCSVWriter::WriteURLRedirects(std::wofstream& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, OutputPlatform platform) {

		unsigned int numberOfChildren = tree->ChildCount(glyph);

		if (!glyph->second.GetURL().empty()) {

			WriteURLRedirect(file, glyph->second.GetURL(), boost::lexical_cast<std::wstring>(m_labelToANTzIDMap[glyph->first]), platform);
		}

		for (unsigned int i = 0; i < numberOfChildren; ++i) {

			WriteURLRedirects(file, tree, tree->GetChild(glyph, i), platform);
		}
	}

	void ANTzCSVWriter::WriteURLRedirect(std::wofstream& file, const std::wstring& url, const std::wstring& id, OutputPlatform platform) {

		if (IsPlatformWindows(platform)) {

			file << L"if %1==" << id << L" (\n\tstart \"\" \"" << url << L"\"\n\texit\n)\n";
		}
		else {


		}
	}

	unsigned long ANTzCSVWriter::WriteHeaders(SynGlyphX::CSVFileWriter& nodeFile, SynGlyphX::CSVFileWriter& tagFile, const std::vector<ANTzGrid>& grids) {

		if (tagFile.IsOpen()) {

			tagFile.WriteLine(m_tagHeaders);
		}

		//Write out header, cameras, and grid lines
		nodeFile.WriteLine(m_nodeHeaders);
		nodeFile.WriteLine(m_cameras[0]);
		nodeFile.WriteLine(m_cameras[1]);
		nodeFile.WriteLine(m_cameras[2]);
		nodeFile.WriteLine(m_cameras[3]);
		nodeFile.WriteLine(m_cameras[4]);

		return WriteGrids(nodeFile, grids, 6);
	}

	void ANTzCSVWriter::WriteGlobals(const std::string& filename, const SynGlyphX::GlyphColor& backgroundColor, OutputPlatform platform) {

		if (filename.empty()) {

			throw std::invalid_argument("Globals filename is empty.");
		}

		try {

			std::wofstream globalsFile;
			globalsFile.open(filename.c_str());

			if (!globalsFile.is_open()) {

				throw std::exception("Globals file failed to open");
			}

			globalsFile.setf(std::ios::fixed, std::ios::floatfield);
			globalsFile.precision(6);

			globalsFile << L"np_globals_id,map_path,item_id,element,type,permisions,name,desc,value" << std::endl;
			globalsFile << L"1,\"np_gl\",1,\"alpha_mode\",\"i\",0,\"\",\"\",\"1\"" << std::endl;
			globalsFile << L"2,\"np_gl\",1,\"background_rgba\",\"ffff\",0,\"\",\"\",\"" << backgroundColor[0] / 255.0 << "," << backgroundColor[1] / 255.0 << "," << backgroundColor[2] / 255.0 << ",1.000000\"" << std::endl;
			globalsFile << L"3,\"np_gl\",1,\"fullscreen\",\"i\",0,\"\",\"\",\"1\"" << std::endl;
			globalsFile << L"4,\"np_gl\",1,\"window_size_xy\",\"ii\",0,\"\",\"\",\"800, 510\"" << std::endl;
			globalsFile << L"5,\"np_gl\",1,\"position_xy\",\"ii\",0,\"\",\"\",\"40, 40\"" << std::endl;
			globalsFile << L"6,\"np_gl\",1,\"hud_level\",\"i\",0,\"\",\"\",\"2\"" << std::endl;
			globalsFile << L"7,\"np_gl\",1,\"subsample\",\"i\",0,\"\",\"\",\"1\"" << std::endl;
			globalsFile << L"8,\"np_mouse\",1,\"tool\",\"i\",0,\"\",\"\",\"4\"" << std::endl;
			globalsFile << L"9,\"np_mouse\",1,\"cam_mode\",\"i\",0,\"\",\"\",\"0\"" << std::endl;
			globalsFile << L"10,\"np_mouse\",1,\"pick_mode\",\"i\",0,\"\",\"\",\"3\"" << std::endl;
			globalsFile << L"11,\"np_db\",1,\"host_ip\",\"s\",0,\"\",\"\",\"127.0.0.1\"" << std::endl;
			globalsFile << L"12,\"np_db\",1,\"user\",\"s\",0,\"\",\"\",\"root\"" << std::endl;
			globalsFile << L"13,\"np_db\",1,\"password\",\"s\",0,\"\",\"\",\"admin\"" << std::endl;
			globalsFile << L"14,\"np_db\",1,\"db_type\",\"s\",0,\"\",\"\",\"mysql\"" << std::endl;
			globalsFile << L"15,\"np_osc\",1,\"tx_ip\",\"s\",0,\"\",\"\",\"127.0.0.1\"" << std::endl;
			globalsFile << L"16,\"np_osc\",1,\"rx_ip\",\"s\", 0,\"\",\"\",\"127.0.0.1\"" << std::endl;
			globalsFile << L"17,\"np_osc\",1,\"tx_port\",\"i\",0,\"\",\"\",\"8000\"" << std::endl;
			globalsFile << L"18,\"np_osc\",1,\"rx_port\",\"i\",0,\"\",\"\",\"9000\"" << std::endl;
			
			if (platform == OutputPlatform::WindowsZSpace) {

				globalsFile << L"19,\"np_browser\",1,\"url\",\"s\",0,\"\",\"\",\"/min redirect.bat \"" << std::endl;
			}
			else {

				globalsFile << L"19,\"np_browser\",1,\"url\",\"s\",0,\"\",\"\",\"http://openantz.com/docs/id.html?id=\"" << std::endl;
			}
			globalsFile << L"20,\"np_globals\",1,\"item_count\",\"i\",1,\"\",\"\",\"20\"" << std::endl;

			globalsFile.close();

		}
		catch (const std::exception& e) {

			throw;
		}
	}

	void ANTzCSVWriter::WriteGlyphTag(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, bool outputURL) {

		unsigned int numberOfChildren = tree->ChildCount(glyph);

		std::wstring tag = L"\"";

		if (outputURL) {

			std::wstring url = m_noURLLocation;
			if (!glyph->second.GetURL().empty()) {

				url = glyph->second.GetURL();
			}
			
			tag += L"<a href=\"" + url + L"\">";
		}

		if (glyph->second.GetTag().empty()) {

			tag += L"No Tag";
		}
		else {

			tag += glyph->second.GetTag();
		}

		if (outputURL) {

			tag += L"</a>";
		}

		tag += L"\"";

		SynGlyphX::CSVFileHandler::CSVValues values;
		values.push_back(boost::lexical_cast<std::wstring>(m_numTagsWritten++));
		values.push_back(boost::lexical_cast<std::wstring>(m_labelToANTzIDMap[glyph->first]));
		values.push_back(L"0");
		values.push_back(tag);
		values.push_back(L"\"" + glyph->second.GetDescription() + L"\"");

		file.WriteLine(values);

		for (unsigned int i = 0; i < numberOfChildren; ++i) {

			WriteGlyphTag(file, tree, tree->GetChild(glyph, i), outputURL);
		}
	}

	unsigned long ANTzCSVWriter::WriteGlyphAndChildren(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel, bool isLink) {

		unsigned long childId = WriteGlyph(file, tree, glyph, id, parentId, 0, branchLevel, isLink);
		
		for (unsigned int i = 0; i < tree->ChildCount(glyph); ++i) {

			childId = WriteGlyphAndChildren(file, tree, tree->GetChild(glyph, i), childId, id, branchLevel + 1, isLink);
		}

		return childId;
	}

	unsigned long ANTzCSVWriter::WriteGlyph(SynGlyphX::CSVFileWriter& file, const SynGlyphX::GlyphGraph::ConstSharedPtr tree, const SynGlyphX::GlyphGraph::ConstGlyphIterator& glyph, unsigned long id, unsigned long parentId, unsigned long childId, unsigned long branchLevel, bool isLink) {

		m_labelToANTzIDMap[glyph->first] = id;
		m_labelToANTzBranchLevelMap[glyph->first] = tree->GetDepth(glyph) - 1;

		return WriteGlyph(file, glyph->second, tree->ChildCount(glyph), id, parentId, childId, branchLevel, isLink);
	}

	unsigned long ANTzCSVWriter::WriteGlyph(SynGlyphX::CSVFileWriter& file, const SynGlyphX::Glyph& glyph, unsigned long numberOfChildren, unsigned long id, unsigned long parentId, unsigned long childId, unsigned long branchLevel, bool isLink) {

		std::wstring idString = boost::lexical_cast<std::wstring>(id);
		SynGlyphX::CSVFileHandler::CSVValues values;
		values.push_back(idString);

		if (isLink) {

			values.push_back(L"7");
		}
		else {

			values.push_back(L"5");
		}
		
		values.push_back(idString);
		values.push_back(L"0");
		values.push_back(boost::lexical_cast<std::wstring>(parentId));
		values.push_back(boost::lexical_cast<std::wstring>(branchLevel));
		values.push_back(boost::lexical_cast<std::wstring>(childId));
		values.push_back(L"0");
		values.push_back(boost::lexical_cast<std::wstring>(numberOfChildren));
		values.insert(values.end(), { L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0" });

		SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(values, glyph.GetScale());
		SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(values, glyph.GetPosition());
		SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(values, glyph.GetTagOffset());
		SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(values, glyph.GetRotationRate());
		SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(values, glyph.GetRotation());
		values.insert(values.end(), { L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0" });

		values.push_back(boost::lexical_cast<std::wstring>(GlyphNodeConverter::ConvertGeometryToNodeValue(glyph.GetStructure().GetGeometryShape(), glyph.GetStructure().GetGeometrySurface())));
		values.push_back(L"1");
		values.push_back(L"0");
		values.push_back(boost::lexical_cast<std::wstring>(glyph.GetStructure().GetTorusRatio()));

		SynGlyphX::GlyphColor color = glyph.GetColor();
		values.push_back(boost::lexical_cast<std::wstring>(GetColorIndex(color)));
		values.push_back(boost::lexical_cast<std::wstring>(color[0]));
		values.push_back(boost::lexical_cast<std::wstring>(color[1]));
		values.push_back(boost::lexical_cast<std::wstring>(color[2]));

		values.push_back(boost::lexical_cast<std::wstring>(glyph.GetTransparency()));

		values.insert(values.end(), { L"0", L"0", L"0", L"0" });
		values.push_back(boost::lexical_cast<std::wstring>(static_cast<int>(glyph.GetVirtualTopology().GetType())));

		values.insert(values.end(), { L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"16", L"16", L"0", L"0", L"0", L"0" });
		values.push_back(idString);
		values.push_back(L"420");

		file.WriteLine(values);

		return id + 1;
	}

	unsigned long ANTzCSVWriter::WriteGrids(SynGlyphX::CSVFileWriter& file, const std::vector<ANTzGrid>& grids, unsigned long firstId) {

		for (unsigned int i = 0; i < grids.size(); ++i) {

			std::wstring id = boost::lexical_cast<std::wstring>(i + firstId);
			SynGlyphX::CSVFileHandler::CSVValues grid;
			grid.push_back(id);
			grid.push_back(L"6");
			grid.push_back(id);
			grid.push_back(L"1");

			if (i == 0) {

				grid.insert(grid.end(), { L"0", L"0", L"0", L"0" });
				grid.push_back(boost::lexical_cast<std::wstring>(grids.size() - 1));
			}
			else {

				grid.push_back(boost::lexical_cast<std::wstring>(firstId));
				grid.insert(grid.end(), { L"1", L"0", L"0", L"0" });
			}
			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"1" });

			//grid cell size
			grid.insert(grid.end(), { L"30", L"30", L"30" });

			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0" });
			SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(grid, grids[i].GetScale());
			SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(grid, grids[i].GetPosition());
			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"0", L"0" });
			SynGlyphX::CSVFileHandler::AddVector3ToCSVValues(grid, grids[i].GetRotation());
			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"1", L"0", L"0.1" });

			SynGlyphX::GlyphColor color = grids[i].GetColor();
			grid.push_back(boost::lexical_cast<std::wstring>(GetColorIndex(color)));
			grid.push_back(boost::lexical_cast<std::wstring>(color[0]));
			grid.push_back(boost::lexical_cast<std::wstring>(color[1]));
			grid.push_back(boost::lexical_cast<std::wstring>(color[2]));
			grid.push_back(L"255");

			grid.push_back(L"0");

			grid.push_back(boost::lexical_cast<std::wstring>(grids[i].GetTextureID()));

			if (grids[i].GetVisible()) {

				grid.push_back(L"0");
			}
			else {

				grid.push_back(L"1");
			}

			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0", L"0" });
			
			//segments
			grid.push_back(L"12");
			grid.push_back(L"6");
				
			grid.insert(grid.end(), { L"0", L"0", L"0", L"0", L"0", L"420" });
			
			file.WriteLine(grid);
		}

		return firstId + grids.size();
	}

	ANTzCSVWriter& ANTzCSVWriter::GetInstance() {
		return s_instance;
	}

	unsigned short ANTzCSVWriter::GetColorIndex(const SynGlyphX::GlyphColor& color) {

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

	const SynGlyphX::CSVFileReader::CSVValues& ANTzCSVWriter::GetNodeHeaders() const {

		return m_nodeHeaders;
	}

	const SynGlyphX::CSVFileReader::CSVValues& ANTzCSVWriter::GetTagHeaders() const {

		return m_tagHeaders;
	}

	const SynGlyphX::CSVFileReader::CSVValues& ANTzCSVWriter::GetChannelHeaders() const {

		return m_channelHeaders;
	}

	const SynGlyphX::CSVFileReader::CSVValues& ANTzCSVWriter::GetChannelMapHeaders() const {

		return m_channelMapHeaders;
	}

	bool ANTzCSVWriter::IsPlatformWindows(OutputPlatform platform) const {

		return ((platform == OutputPlatform::Windows) || (platform == OutputPlatform::WindowsZSpace));
	}

} // namespace SynGlyphXANTz