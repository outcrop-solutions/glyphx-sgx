#include "glyphnodeconverter.h"
#include <boost/lexical_cast.hpp>
#include "csvfilereader.h"
#include "antzcsvwriter.h"
#include "data/nptypes.h"

namespace SynGlyphXANTz {

	GlyphNodeConverter::GlyphNodeConverter()
	{
	}


	GlyphNodeConverter::~GlyphNodeConverter()
	{
	}

	SynGlyphX::Glyph GlyphNodeConverter::CreateGlyphFromNode(pNPnode node) {

		SynGlyphX::Glyph glyph;

		glyph.GetTagOffset()[0] = node->tagOffset.x;
		glyph.GetTagOffset()[1] = node->tagOffset.y;
		glyph.GetTagOffset()[2] = node->tagOffset.z;

		glyph.GetScale()[0] = node->scale.x;
		glyph.GetScale()[1] = node->scale.y;
		glyph.GetScale()[2] = node->scale.z;

		glyph.GetPosition()[0] = node->translate.x;
		glyph.GetPosition()[1] = node->translate.y;
		glyph.GetPosition()[2] = node->translate.z;

		glyph.GetRotation()[0] = node->rotate.x;
		glyph.GetRotation()[1] = node->rotate.y;
		glyph.GetRotation()[2] = node->rotate.z;

		glyph.GetColor().Set(0, node->color.r);
		glyph.GetColor().Set(1, node->color.g);
		glyph.GetColor().Set(2, node->color.b);
		glyph.GetTransparency() = node->color.a;

		glyph.GetTag() = GetTag(node);
		glyph.GetURL() = GetUrl(node);

		if (node->tag->descSize == 0) {

			glyph.GetDescription() = L"";
		}
		else {

			glyph.GetDescription() = boost::lexical_cast<std::wstring>(node->tag->desc);
		}

		glyph.GetRotationRate()[0] = node->rotateRate.x;
		glyph.GetRotationRate()[1] = node->rotateRate.y;
		glyph.GetRotationRate()[2] = node->rotateRate.z;

		SynGlyphX::GlyphGeometryInfo::Shape shape;
		SynGlyphX::GlyphGeometryInfo::Surface surface;
		GetShapeAndSurfaceFromNodeValue(node->geometry, shape, surface);

		glyph.GetStructure().SetGeometryShape(shape);
		glyph.GetStructure().SetGeometrySurface(surface);

		glyph.GetVirtualTopology().SetType(static_cast<SynGlyphX::VirtualTopologyInfo::Type>(node->topo));

		return glyph;
	}

	unsigned int GlyphNodeConverter::ConvertGeometryToNodeValue(SynGlyphX::GlyphGeometryInfo::Shape shape, SynGlyphX::GlyphGeometryInfo::Surface surface) {

		//This is necessary because the enum for geometries in ANTz is screwed up
		if (shape == SynGlyphX::GlyphGeometryInfo::Shape::Pin) {

			return (2 * shape) + 1 - surface;
		}
		else {

			return (2 * shape) + surface;
		}
	}

	void GlyphNodeConverter::GetShapeAndSurfaceFromNodeValue(unsigned int geometry, SynGlyphX::GlyphGeometryInfo::Shape& shape, SynGlyphX::GlyphGeometryInfo::Surface& surface) {

		shape = static_cast<SynGlyphX::GlyphGeometryInfo::Shape>(geometry / 2);

		//This is necessary because the enum for geometries in ANTz is screwed up
		if (shape == SynGlyphX::GlyphGeometryInfo::Shape::Pin) {

			surface = static_cast<SynGlyphX::GlyphGeometryInfo::Surface>(1 - (geometry % 2));
		}
		else {

			surface = static_cast<SynGlyphX::GlyphGeometryInfo::Surface>(geometry % 2);
		}
	}

	SynGlyphX::GlyphGraph::SharedPtr GlyphNodeConverter::CreateGlyphGraphFromCSV(const std::string& filename) {

		SynGlyphX::GlyphGraph::SharedPtr glyphGraph = std::make_shared<SynGlyphX::GlyphGraph>();

		SynGlyphX::CSVFileReader csvReader(filename.c_str());

		if (!AreHeadersCorrect(csvReader.GetHeaders())) {

			throw std::invalid_argument("Could not read CSV file into glyph tree: Invalid Headers");
		}

		SynGlyphX::CSVFileHandler::CSVValues currentLineValues;
		while (!csvReader.IsAtEndOfFile()) {

			currentLineValues = csvReader.GetValuesFromLine(true);
			if (currentLineValues[1] == L"5") {

				break;
			}
		}

		if ((currentLineValues.empty()) || (currentLineValues[1] != L"5")) {

			throw std::invalid_argument("CSV file has no glyph");
		}

		std::unordered_map<std::wstring, SynGlyphX::GlyphGraph::GlyphIterator> indexToNodeMap;

		indexToNodeMap[currentLineValues[0]] = glyphGraph->SetRootGlyph(CreateGlyphFromCSVValues(currentLineValues));
		currentLineValues = csvReader.GetValuesFromLine(true);

		do {
			if (currentLineValues[5] == L"0") {

				break;
			}

			if (currentLineValues[1] == L"5") {

				indexToNodeMap[currentLineValues[0]] = glyphGraph->AddChildGlyph(indexToNodeMap[currentLineValues[4]], CreateGlyphFromCSVValues(currentLineValues));
			}
			else if (currentLineValues[1] == L"7") {

				SynGlyphX::GlyphGraph::Label source = indexToNodeMap[currentLineValues[4]]->first;
				SynGlyphX::GlyphGraph::Label destination = indexToNodeMap[currentLineValues[6]]->first;
				glyphGraph->AddLink(source, destination, CreateGlyphFromCSVValues(currentLineValues));
			}

			currentLineValues = csvReader.GetValuesFromLine(true);

		} while (!csvReader.IsAtEndOfFile());

		return glyphGraph;
	}

	SynGlyphX::Glyph GlyphNodeConverter::CreateGlyphFromCSVValues(const SynGlyphX::CSVFileHandler::CSVValues& csvValues) {

		SynGlyphX::Glyph glyph;

		glyph.GetTagOffset()[0] = boost::lexical_cast<double>(csvValues[31]);
		glyph.GetTagOffset()[1] = boost::lexical_cast<double>(csvValues[32]);
		glyph.GetTagOffset()[2] = boost::lexical_cast<double>(csvValues[33]);

		glyph.GetScale()[0] = boost::lexical_cast<double>(csvValues[25]);
		glyph.GetScale()[1] = boost::lexical_cast<double>(csvValues[26]);
		glyph.GetScale()[2] = boost::lexical_cast<double>(csvValues[27]);

		glyph.GetPosition()[0] = boost::lexical_cast<double>(csvValues[28]);
		glyph.GetPosition()[1] = boost::lexical_cast<double>(csvValues[29]);
		glyph.GetPosition()[2] = boost::lexical_cast<double>(csvValues[30]);

		glyph.GetRotation()[0] = boost::lexical_cast<double>(csvValues[37]);
		glyph.GetRotation()[1] = boost::lexical_cast<double>(csvValues[38]);
		glyph.GetRotation()[2] = boost::lexical_cast<double>(csvValues[39]);

		glyph.GetColor().Set(0, boost::lexical_cast<double>(csvValues[55]));
		glyph.GetColor().Set(1, boost::lexical_cast<double>(csvValues[56]));
		glyph.GetColor().Set(2, boost::lexical_cast<double>(csvValues[57]));
		glyph.GetTransparency() = boost::lexical_cast<double>(csvValues[58]);

		SynGlyphX::GlyphGeometryInfo::Shape shape;
		SynGlyphX::GlyphGeometryInfo::Surface surface;
		GetShapeAndSurfaceFromNodeValue(boost::lexical_cast<unsigned int>(csvValues[50]), shape, surface);

		glyph.GetStructure().SetGeometryShape(shape);
		glyph.GetStructure().SetGeometrySurface(surface);
		glyph.GetStructure().SetTorusRatio(boost::lexical_cast<double>(csvValues[53]));

		SynGlyphX::VirtualTopologyInfo::Type virtualTopologyType = static_cast<SynGlyphX::VirtualTopologyInfo::Type>(boost::lexical_cast<unsigned int>(csvValues[63]));
		if (virtualTopologyType == SynGlyphX::VirtualTopologyInfo::Type::Null) {

			glyph.GetVirtualTopology().SetType(SynGlyphX::VirtualTopologyInfo::Type::Circle);
		}
		else {

			glyph.GetVirtualTopology().SetType(virtualTopologyType);
		}

		glyph.GetRotationRate()[0] = boost::lexical_cast<double>(csvValues[34]);
		glyph.GetRotationRate()[1] = boost::lexical_cast<double>(csvValues[35]);
		glyph.GetRotationRate()[2] = boost::lexical_cast<double>(csvValues[36]);

		return glyph;
	}

	bool GlyphNodeConverter::AreHeadersCorrect(const SynGlyphX::CSVFileHandler::CSVValues& headersFromFile) {

		const SynGlyphX::CSVFileHandler::CSVValues& standardHeaders = ANTzCSVWriter::GetInstance().GetNodeHeaders();
		if (standardHeaders.size() != headersFromFile.size()) {

			return false;
		}

		for (int i = 0; i < headersFromFile.size(); ++i) {

			if (standardHeaders[i] != headersFromFile[i]) {

				if ((i != 13) || (headersFromFile[i] != L"interval")){

					return false;
				}
			}
		}

		return true;
	}

	std::wstring GlyphNodeConverter::GetTag(pNPnode node) {

		if (node->tag->titleSize == 0) {

			return L"";
		}

		std::wstring tag = boost::lexical_cast<std::wstring>(node->tag->title);
		
		if (tag.substr(0, 9) == L"<a href=\"") {

			int pos = tag.find_first_of('\"', 9) + 2;
			return tag.substr(pos, tag.length() - pos - 4);
		}
		else {

			return tag;
		}
	}

	std::wstring GlyphNodeConverter::GetUrl(pNPnode node) {

		if (node->tag->titleSize == 0) {

			return L"";
		}

		std::wstring tag = boost::lexical_cast<std::wstring>(node->tag->title);

		if (tag.substr(0, 9) == L"<a href=\"") {

			return tag.substr(9, tag.find_first_of('\"', 9) - 9);
		}
		else {

			return L"";
		}
	}

} //namespace SynGlyphXANTz