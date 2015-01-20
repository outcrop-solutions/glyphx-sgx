#include "glyphnodeconverter.h"
#include <boost/lexical_cast.hpp>
#include "csvfilereader.h"
#include "antzcsvwriter.h"

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

		glyph.GetTag() = boost::lexical_cast<std::wstring>(node->tag->title);
		glyph.GetDescription() = boost::lexical_cast<std::wstring>(node->tag->desc);

		glyph.GetRotationRate()[0] = node->rotateRate.x;
		glyph.GetRotationRate()[1] = node->rotateRate.y;
		glyph.GetRotationRate()[2] = node->rotateRate.z;

		SynGlyphX::GlyphStructuralProperties::Shape shape;
		SynGlyphX::GlyphStructuralProperties::Surface surface;
		GetShapeAndSurfaceFromNodeValue(node->geometry, shape, surface);

		glyph.GetStructure().SetGeometryShape(shape);
		glyph.GetStructure().SetGeometrySurface(surface);

		glyph.GetStructure().SetVirtualTopology(static_cast<SynGlyphX::GlyphStructuralProperties::VirtualTopology>(node->topo));

		return glyph;
	}

	unsigned int GlyphNodeConverter::ConvertGeometryToNodeValue(SynGlyphX::GlyphStructuralProperties::Shape shape, SynGlyphX::GlyphStructuralProperties::Surface surface) {

		//This is necessary because the enum for geometries in ANTz is screwed up
		if (shape == SynGlyphX::GlyphStructuralProperties::Shape::Pin) {

			return (2 * shape) + 1 - surface;
		}
		else {

			return (2 * shape) + surface;
		}
	}

	void GlyphNodeConverter::GetShapeAndSurfaceFromNodeValue(unsigned int geometry, SynGlyphX::GlyphStructuralProperties::Shape& shape, SynGlyphX::GlyphStructuralProperties::Surface& surface) {

		shape = static_cast<SynGlyphX::GlyphStructuralProperties::Shape>(geometry / 2);

		//This is necessary because the enum for geometries in ANTz is screwed up
		if (shape == SynGlyphX::GlyphStructuralProperties::Shape::Pin) {

			surface = static_cast<SynGlyphX::GlyphStructuralProperties::Surface>(1 - (geometry % 2));
		}
		else {

			surface = static_cast<SynGlyphX::GlyphStructuralProperties::Surface>(geometry % 2);
		}
	}

	SynGlyphX::GlyphGraph::SharedPtr GlyphNodeConverter::CreateGlyphGraphFromCSV(const std::string& filename) {

		SynGlyphX::GlyphGraph::SharedPtr glyphGraph = std::make_shared<SynGlyphX::GlyphGraph>();

		SynGlyphX::CSVFileReader csvReader(filename);

		if (csvReader.GetHeaders() != ANTzCSVWriter::GetInstance().GetNodeHeaders()) {

			throw std::invalid_argument("Could not read CSV file into glyph tree: Invalid Headers");
		}

		SynGlyphX::CSVFileHandler::CSVValues currentLineValues;
		while (!csvReader.IsAtEndOfFile()) {

			currentLineValues = csvReader.GetValuesFromLine();
			if (currentLineValues[1] == L"5") {

				break;
			}
		}

		if ((currentLineValues.empty()) || (currentLineValues[1] != L"5")) {

			throw std::invalid_argument("CSV file has no glyph");
		}

		std::unordered_map<std::wstring, SynGlyphX::GlyphGraph::iterator> indexToNodeMap;

		indexToNodeMap[currentLineValues[0]] = glyphGraph->insert(CreateGlyphFromCSVValues(currentLineValues));
		currentLineValues = csvReader.GetValuesFromLine();

		do {
			if (currentLineValues[5] == L"0") {

				break;
			}

			if (currentLineValues[1] != L"5") {

				currentLineValues = csvReader.GetValuesFromLine();
				continue;
			}

			indexToNodeMap[currentLineValues[0]] = glyphGraph->insert(indexToNodeMap[currentLineValues[4]], CreateGlyphFromCSVValues(currentLineValues));
			currentLineValues = csvReader.GetValuesFromLine();

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

		SynGlyphX::GlyphStructuralProperties::Shape shape;
		SynGlyphX::GlyphStructuralProperties::Surface surface;
		GetShapeAndSurfaceFromNodeValue(boost::lexical_cast<unsigned int>(csvValues[50]), shape, surface);

		glyph.GetStructure().SetGeometryShape(shape);
		glyph.GetStructure().SetGeometrySurface(surface);

		glyph.GetStructure().SetVirtualTopology(static_cast<SynGlyphX::GlyphStructuralProperties::VirtualTopology>(boost::lexical_cast<unsigned int>(csvValues[63])));

		glyph.GetRotationRate()[0] = boost::lexical_cast<double>(csvValues[34]);
		glyph.GetRotationRate()[1] = boost::lexical_cast<double>(csvValues[35]);
		glyph.GetRotationRate()[2] = boost::lexical_cast<double>(csvValues[36]);

		return glyph;
	}

} //namespace SynGlyphXANTz