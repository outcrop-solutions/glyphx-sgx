#ifndef SYNGLYPHX_CSVREADERWRITER
#define SYNGLYPHX_CSVREADERWRITER

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "glyph.h"

namespace SynGlyphX {

    const unsigned short MaxPredefinedColors = 20;

    class CSVReaderWriter : public boost::noncopyable
    {
    public:
        CSVReaderWriter();
        ~CSVReaderWriter();

        void Write(const boost::shared_ptr<Glyph> treeRoot, unsigned long startingIndex = 32);

        

    private:
        unsigned short GetColorIndex(const Color& color);

        Color m_predefinedColors[MaxPredefinedColors];
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVREADERWRITER