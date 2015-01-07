#ifndef SYNGLYPHX_ANTZ_CSVREADERWRITER
#define SYNGLYPHX_ANTZ_CSVREADERWRITER

#include "ANTzBridge.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "glyph.h"
#include "color.h"
#include <array>
#include "csvfilereader.h"
#include "csvfilewriter.h"
#include "antzgrid.h"

namespace SynGlyphXANTz {

    const unsigned short MaxPredefinedColors = 20;

	class ANTZBRIDGE_API ANTzCSVWriter : public boost::noncopyable
    {
    public:
        ANTzCSVWriter();
        ~ANTzCSVWriter();

		void Write(const std::string& nodeFilename, const std::string& tagFilename, const GlyphTree::ConstSharedVector& trees, const std::vector<ANTzGrid>& grids);

        static ANTzCSVWriter& GetInstance();

		const SynGlyphX::CSVFileReader::CSVValues& GetNodeHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetTagHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetChannelHeaders() const;
		const SynGlyphX::CSVFileReader::CSVValues& GetChannelMapHeaders() const;

    private:
        static ANTzCSVWriter s_instance;

		unsigned long WriteNodeFile(const std::string& filename, const GlyphTree::ConstSharedVector& trees, const std::vector<ANTzGrid>& grids);
		void WriteTagFile(const std::string& filename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId);

		unsigned long WriteGlyph(SynGlyphX::CSVFileWriter& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel);
		unsigned long WriteGlyphTag(SynGlyphX::CSVFileWriter& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id);
		unsigned long WriteGrids(SynGlyphX::CSVFileWriter& file, const std::vector<ANTzGrid>& grids, unsigned long firstId);
		unsigned int ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface);
        unsigned short GetColorIndex(const SynGlyphX::Color& color);

		SynGlyphX::Color m_predefinedColors[MaxPredefinedColors];
		unsigned long m_numTagsWritten;

		SynGlyphX::CSVFileReader::CSVValues m_nodeHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_tagHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_channelHeaders;
		SynGlyphX::CSVFileReader::CSVValues m_channelMapHeaders;

		SynGlyphX::CSVFileReader::CSVValues m_cameras[5];
    };

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_ANTZ_CSVREADERWRITER