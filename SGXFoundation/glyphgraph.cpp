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

	const Glyph GlyphGraph::s_defaultGlyph = GlyphGraph::CreateDefaultGlyph(GlyphStructuralProperties::Shape::Torus, GlyphStructuralProperties::VirtualTopology::Circle);
	const Glyph GlyphGraph::s_defaultRootGlyph = GlyphGraph::CreateDefaultGlyph(GlyphStructuralProperties::Shape::Pin, GlyphStructuralProperties::VirtualTopology::LinePin);

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

		if (instances.size() == 1) {

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

	Glyph GlyphGraph::CreateDefaultGlyph(GlyphStructuralProperties::Shape geometryShape, GlyphStructuralProperties::VirtualTopology virtualTopology) {

		Glyph glyph;

		glyph.GetStructure().SetGeometryShape(geometryShape);
		glyph.GetStructure().SetGeometrySurface(GlyphStructuralProperties::Surface::Solid);
		glyph.GetStructure().SetVirtualTopology(virtualTopology);
		
		glyph.GetPosition()[0] = 0.0;
		glyph.GetPosition()[1] = 0.0;
		glyph.GetPosition()[2] = 0.0;
		glyph.GetRotation()[0] = 0.0;
		glyph.GetRotation()[1] = 0.0;
		glyph.GetRotation()[2] = 0.0;
		glyph.GetScale()[0] = 1.0;
		glyph.GetScale()[1] = 1.0;
		glyph.GetScale()[2] = 1.0;
		glyph.GetColor() = GlyphColor({ { 50, 101, 101 } });
		glyph.GetTransparency() = 255;

		glyph.GetTag() = L"";
		glyph.GetDescription() = L"";

		glyph.GetRotationRate()[0] = 0.0;
		glyph.GetRotationRate()[1] = 0.0;
		glyph.GetRotationRate()[2] = 0.0;

		return glyph;
	}

} //namespace SynGlyphX