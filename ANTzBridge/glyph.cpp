#include "glyph.h"
#include "data/nptypes.h"

namespace SynGlyphX {

	GlyphTree::GlyphTree()
		: stlplus::ntree<GlyphProperties>() {

	}

	GlyphTree::GlyphTree(pNPnode node) :
		stlplus::ntree<GlyphProperties>() {

		GlyphProperties rootProperties(node);
		insert(rootProperties);

		for (int i = 0; i < node->childCount; ++i) {

			GlyphTree childTree(node->child[i]);
			insert(root(), childTree);
		}
	}

	GlyphTree::GlyphTree(const GlyphProperties& rootProperties) :
		stlplus::ntree<GlyphProperties>() {

		insert(rootProperties);
	}

	GlyphTree::GlyphTree(const std::vector<SynGlyphX::GlyphProperties::ConstSharedPtr>& templates, const std::vector<unsigned int> instances, unsigned int depth) :
		stlplus::ntree<GlyphProperties>() {

		insert(*templates[depth]);
		++depth;

		if (depth == instances.size() - 1) {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				insert(root(), *templates[depth]);
			}
		}
		else {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphTree childTree(templates, instances, depth);
				insert(root(), childTree);
			}
		}
	}

	GlyphTree::~GlyphTree()
	{
	}

	void GlyphTree::AllocateChildSubtree(const std::vector<SynGlyphX::GlyphProperties::ConstSharedPtr>& templates, const std::vector<unsigned int> instances, const GlyphTree::iterator& parent) {

		if (instances.size() == 1) {

			for (unsigned int i = 0; i < instances[0]; ++i) {

				insert(root(), *templates[0]);
			}
		}
		else {
			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphTree childTree(templates, instances, 0);
				insert(parent, childTree);
			}
		}
	}

    /*Glyph::Glyph()
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
    }*/

} //namespace SynGlyphX