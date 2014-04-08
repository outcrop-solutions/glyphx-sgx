#include "cvsreaderwriter.h"

namespace SynGlyphX {

    CSVReaderWriter::CSVReaderWriter()
    {
        m_predefinedColors[0] = { { 50, 101, 101, 255 } };
        m_predefinedColors[1] = { { 0, 255, 0, 255 } };
        m_predefinedColors[2] = { { 255, 0, 0, 255 } };
        m_predefinedColors[3] = { { 0, 0, 255, 255 } };
        m_predefinedColors[4] = { { 255, 255, 0, 255 } };
        m_predefinedColors[5] = { { 152, 0, 255, 255 } };
        m_predefinedColors[6] = { { 255, 168, 0, 255 } };
        m_predefinedColors[7] = { { 0, 255, 255, 255 } };
        m_predefinedColors[8] = { { 255, 0, 255, 255 } };
        m_predefinedColors[9] = { { 0, 153, 0, 255 } };
        m_predefinedColors[10] = { { 185, 153, 102, 255 } };
        m_predefinedColors[11] = { { 255, 180, 255, 255 } };
        m_predefinedColors[12] = { { 0, 152, 255, 255 } };
        m_predefinedColors[13] = { { 185, 255, 0, 255 } };
        m_predefinedColors[14] = { { 152, 0, 0, 255 } };
        m_predefinedColors[15] = { { 127, 127, 127, 255 } };
        m_predefinedColors[16] = { { 127, 127, 255, 255 } };
        m_predefinedColors[17] = { { 197, 82, 0, 255 } };
        m_predefinedColors[18] = { { 0, 0, 0, 255 } };
        m_predefinedColors[19] = { { 255, 255, 255, 255 } };
    }


    CSVReaderWriter::~CSVReaderWriter()
    {
    }

    void CSVReaderWriter::Write(const boost::shared_ptr<Glyph> treeRoot, unsigned long startingIndex) {

    }

    unsigned short CSVReaderWriter::GetColorIndex(const Color& color) {

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

} // namespace SynGlyphX