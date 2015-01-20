#ifndef SYNGLYPHX_ANTZ_GLYPHNODECONVERTER
#define SYNGLYPHX_ANTZ_GLYPHNODECONVERTER

#include "ANTzBridge.h"
#include "glyphgraph.h"
#include "data/nptypes.h"
#include "csvfilehandler.h"

namespace SynGlyphXANTz {

	class ANTZBRIDGE_API GlyphNodeConverter
	{
	public:
		GlyphNodeConverter();
		~GlyphNodeConverter();

		static SynGlyphX::Glyph CreateGlyphFromNode(pNPnode node);
		static unsigned int ConvertGeometryToNodeValue(SynGlyphX::GlyphStructuralProperties::Shape shape, SynGlyphX::GlyphStructuralProperties::Surface surface);
		static void GetShapeAndSurfaceFromNodeValue(unsigned int geometry, SynGlyphX::GlyphStructuralProperties::Shape& shape, SynGlyphX::GlyphStructuralProperties::Surface& surface);
		static SynGlyphX::GlyphGraph::SharedPtr CreateGlyphGraphFromCSV(const std::string& filename);

	private:
		static SynGlyphX::Glyph CreateGlyphFromCSVValues(const SynGlyphX::CSVFileHandler::CSVValues& csvValues);
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_GLYPHNODECONVERTER