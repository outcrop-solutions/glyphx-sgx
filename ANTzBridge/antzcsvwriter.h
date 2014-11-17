#ifndef SYNGLYPHX_CSVREADERWRITER
#define SYNGLYPHX_CSVREADERWRITER

#include "ANTzBridge.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "glyph.h"
#include "color.h"
#include <array>
#include "csvfilereader.h"
#include "csvfilewriter.h"

namespace SynGlyphX {

    const unsigned short MaxPredefinedColors = 20;

	class ANTZBRIDGE_API ANTzCSVWriter : public boost::noncopyable
    {
    public:
        ANTzCSVWriter();
        ~ANTzCSVWriter();

		void Write(const std::string& nodeFilename, const std::string& tagFilename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId = 32);

        static ANTzCSVWriter& GetInstance();

		const CSVFileReader::CSVValues& GetNodeHeaders() const;
		const CSVFileReader::CSVValues& GetTagHeaders() const;
		const CSVFileReader::CSVValues& GetChannelHeaders() const;
		const CSVFileReader::CSVValues& GetChannelMapHeaders() const;

    private:
        static ANTzCSVWriter s_instance;

		unsigned long WriteNodeFile(const std::string& filename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId);
		void WriteTagFile(const std::string& filename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId);

		unsigned long WriteGlyph(CSVFileWriter& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel);
		unsigned long WriteGlyphTag(CSVFileWriter& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id);
		unsigned long WriteGrids(CSVFileWriter& file, unsigned long id);
		unsigned int ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface);
        unsigned short GetColorIndex(const Color& color);

		Color m_predefinedColors[MaxPredefinedColors];
		unsigned long m_numTagsWritten;

		CSVFileReader::CSVValues m_nodeHeaders;
		CSVFileReader::CSVValues m_tagHeaders;
		CSVFileReader::CSVValues m_channelHeaders;
		CSVFileReader::CSVValues m_channelMapHeaders;

		CSVFileReader::CSVValues m_cameras[5];
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVREADERWRITER