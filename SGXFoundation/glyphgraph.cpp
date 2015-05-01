#include "glyphgraph.h"

namespace SynGlyphX {

	GlyphEdge::GlyphEdge() : 
		m_type(Type::Topological) {

	}

	GlyphEdge::GlyphEdge(const GlyphEdge& edge) :
		m_type(edge.m_type) {

	}

	GlyphEdge::~GlyphEdge() {

	}

	GlyphGraph::GlyphGraph() :
		stlplus::ntree<Glyph>() {


	}

	GlyphGraph::GlyphGraph(const Glyph& rootGlyph) :
		stlplus::ntree<Glyph>() {

		insert(rootGlyph);
	}

	GlyphGraph::GlyphGraph(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, unsigned int depth) :
		stlplus::ntree<Glyph>() {

		insert(templates[depth]);
		++depth;

		if (depth == instances.size() - 1) {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphGraph::iterator newGlyph = insert(root(), templates[depth]);
				newGlyph->GetPosition()[0] = i * -15.0;
			}
		}
		else {

			for (unsigned int i = 0; i < instances[depth]; ++i) {

				GlyphGraph subGraph(templates, instances, depth);
				GlyphGraph::iterator newGlyph = insert(root(), subGraph);
				newGlyph->GetPosition()[0] = i * -15.0;
			}
		}
	}

	GlyphGraph::GlyphGraph(const GlyphGraph& graph) :
		stlplus::ntree<Glyph>(graph) {


	}

	GlyphGraph::~GlyphGraph() {


	}

	void GlyphGraph::AllocateChildSubtree(const std::vector<Glyph>& templates, const std::vector<unsigned int> instances, const GlyphGraph::iterator& parent) {

		unsigned int numberOfInstances = instances.size();
		if ((numberOfInstances == 0) || (templates.empty())) {

			return;
		}
		else if (numberOfInstances == 1) {

			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphGraph::iterator newGlyph = insert(root(), templates[0]);
				newGlyph->GetPosition()[0] = i * -15.0;
			}
		}
		else {
			for (unsigned int i = 0; i < instances[0]; ++i) {

				GlyphGraph subGraph(templates, instances, 0);
				GlyphGraph::iterator newGlyph = insert(parent, subGraph);
				newGlyph->GetPosition()[0] = i * -15.0;
			}
		}
	}

} //namespace SynGlyphX