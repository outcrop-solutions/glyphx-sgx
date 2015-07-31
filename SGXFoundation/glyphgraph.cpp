#include "glyphgraph.h"

namespace SynGlyphX {

	GlyphGraph::GlyphGraph() :
		GlyphGraphTemplate<Glyph>() {


	}

	GlyphGraph::GlyphGraph(const Glyph& rootGlyph) :
		GlyphGraphTemplate<Glyph>(rootGlyph) {

	}

	GlyphGraph::GlyphGraph(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, unsigned int depth) :
		GlyphGraphTemplate<Glyph>() {

		SetRootGlyph(templates[depth]);
		++depth;

		if (depth == instances.size() - 1) {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphGraph::GlyphIterator newGlyph = AddChildGlyph(GetRoot(), templates[depth]);
				newGlyph->second.GetPosition()[0] = i * -15.0;
			}
		}
		else {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphGraph subGraph(templates, instances, depth);
				GlyphGraph::GlyphIterator newGlyph = AddChildGlyphGraph(GetRoot(), subGraph);
				newGlyph->second.GetPosition()[0] = i * -15.0;
			}
		}
	}

	GlyphGraph::GlyphGraph(const GlyphGraph& graph) :
		GlyphGraphTemplate<Glyph>(graph) {


	}

	GlyphGraph::~GlyphGraph() {


	}

	void GlyphGraph::AllocateChildSubtree(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, const GlyphGraph::GlyphIterator& parent) {

		unsigned int numberOfInstances = instances.size();
		if ((numberOfInstances == 0) || (templates.empty())) {

			return;
		}
		else if (numberOfInstances == 1) {

			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphGraph::GlyphIterator newGlyph = AddChildGlyph(GetRoot(), templates[0]);
				newGlyph->second.GetPosition()[0] = i * -15.0;
			}
		}
		else {
			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphGraph subGraph(templates, instances, 0);
				GlyphGraph::GlyphIterator newGlyph = AddChildGlyphGraph(parent, subGraph);
				newGlyph->second.GetPosition()[0] = i * -15.0;
			}
		}
	}

} //namespace SynGlyphX