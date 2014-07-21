#ifndef SYNGLYPHX_CSVREADERWRITER
#define SYNGLYPHX_CSVREADERWRITER

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include "glyph.h"
#include "color.h"
#include <array>

namespace SynGlyphX {

    const unsigned short MaxPredefinedColors = 20;

    class CSVReaderWriter : public boost::noncopyable
    {
    public:
        CSVReaderWriter();
        ~CSVReaderWriter();

        void Write(const std::string& filename, GlyphTree::ConstSharedPtr treeRoot, unsigned long startingId = 32);

        static CSVReaderWriter& GetInstance();

    private:
        static CSVReaderWriter s_instance;

		void WriteGlyph(std::ofstream& file, const GlyphProperties& glyph, unsigned long id, unsigned long parentId, unsigned long branchLevel);
		unsigned int ConvertGeometryToCSVInt(GlyphProperties::Shape shape, GlyphProperties::Surface surface);
        unsigned short GetColorIndex(const Color& color);

		Color m_predefinedColors[MaxPredefinedColors];
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVREADERWRITER