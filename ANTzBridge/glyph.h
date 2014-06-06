#ifndef SYNGLYPHX_GLYPH
#define SYNGLYPHX_GLYPH

#include "ANTzBridge.h"
#include <boost/shared_ptr.hpp>
#include <vector>
#include "glyphproperties.h"

namespace SynGlyphX {

    class ANTZBRIDGE_API Glyph : public GlyphProperties
    {
    public:
        Glyph();
        Glyph(pNPnode node);
        Glyph(const std::vector<boost::shared_ptr<GlyphProperties>>& templates, const std::vector<unsigned int>& numberOfChildren);
        ~Glyph();

        void AddChild(boost::shared_ptr<Glyph> glyph);
        boost::shared_ptr<Glyph> GetChild(unsigned int index) const;

        void ClearChildren();

        virtual unsigned int GetNumberOfChildren() const;

    private:
        std::vector<boost::shared_ptr<Glyph>> m_children;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH