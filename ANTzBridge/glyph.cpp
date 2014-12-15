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

				GlyphTree::iterator newGlyph = insert(root(), *templates[depth]);
				SetPosition(newGlyph, i * -15.0);
			}
		}
		else {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphTree childTree(templates, instances, depth);
				GlyphTree::iterator newGlyph = insert(root(), childTree);
				SetPosition(newGlyph, i * -15.0);
			}
		}
	}

	GlyphTree::~GlyphTree()
	{
	}

	void GlyphTree::AllocateChildSubtree(const std::vector<SynGlyphX::GlyphProperties::ConstSharedPtr>& templates, const std::vector<unsigned int> instances, const GlyphTree::iterator& parent) {

		if (instances.size() == 1) {

			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphTree::iterator newGlyph = insert(root(), *templates[0]);
				SetPosition(newGlyph, i * -15.0);
			}
		}
		else {
			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphTree childTree(templates, instances, 0);
				GlyphTree::iterator newGlyph = insert(parent, childTree);
				SetPosition(newGlyph, i * -15.0);
			}
		}
	}

	void GlyphTree::SetPosition(GlyphTree::iterator& glyph, double xPosition) {

		Vector3 pos = glyph->GetPosition();
		pos[0] = xPosition;
		glyph->SetPosition(pos);
	}

} //namespace SynGlyphX