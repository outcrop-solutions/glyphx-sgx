#ifndef SYNGLYPHX_CSVREADERWRITER
#define SYNGLYPHX_CSVREADERWRITER

#include "ANTzBridge.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "glyph.h"
#include "color.h"
#include <array>

namespace SynGlyphX {

    const unsigned short MaxPredefinedColors = 20;

	class ANTZBRIDGE_API ANTzCSVWriter : public boost::noncopyable
    {
    public:
        ANTzCSVWriter();
        ~ANTzCSVWriter();

		void Write(const std::string& filename, const std::string& tagFilename, const GlyphTree::ConstSharedVector& trees, unsigned long startingId = 32);

        static ANTzCSVWriter& GetInstance();

    private:
        static ANTzCSVWriter s_instance;

		unsigned long WriteGlyph(std::ofstream& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel);
		unsigned long WriteGlyphTag(std::ofstream& file, const GlyphTree::ConstSharedPtr tree, const GlyphTree::const_iterator& glyph, unsigned long id);
		unsigned int ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface);
        unsigned short GetColorIndex(const Color& color);

		Color m_predefinedColors[MaxPredefinedColors];
		unsigned long m_numTagsWritten;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVREADERWRITER