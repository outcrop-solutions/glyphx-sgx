#include "glyph.h"
#include "data/nptypes.h"

namespace SynGlyphX {

    Glyph::Glyph()
        : GlyphProperties() {
        
    }

    Glyph::Glyph(pNPnode node) :
        GlyphProperties(node) {

        if (node->childCount > 0) {

            for (int i = 0; i < node->childCount; ++i) {

                boost::shared_ptr<Glyph> glyph(new SynGlyphX::Glyph(node->child[i]));
                m_children.push_back(glyph);
            }
        }
    }

    Glyph::Glyph(const std::vector<boost::shared_ptr<GlyphProperties>>& templates, const std::vector<unsigned int>& numberOfChildren) :
        GlyphProperties(*(templates[0].get())) {

        std::vector<boost::shared_ptr<GlyphProperties>> childTemplates(templates);
        childTemplates.erase(childTemplates.begin());

        if (!childTemplates.empty()) {

            std::vector<unsigned int> numberOfDesendants(numberOfChildren);
            numberOfDesendants.erase(numberOfDesendants.begin());

            for (int i = 0; i < numberOfChildren[0]; ++i) {

                boost::shared_ptr<Glyph> glyph(new SynGlyphX::Glyph(childTemplates, numberOfDesendants));
                m_children.push_back(glyph);
            }
        }
    }

    Glyph::~Glyph()
    {
    }

    
    void Glyph::AddChild(boost::shared_ptr<Glyph> glyph) {
        m_children.push_back(glyph);
    }

    boost::shared_ptr<Glyph> Glyph::GetChild(unsigned int index) const {
        return m_children[index];
    }
    
    void Glyph::ClearChildren() {
        m_children.clear();
    }

    unsigned int Glyph::GetNumberOfChildren() const {
        return m_children.size();
    }

} //namespace SynGlyphX