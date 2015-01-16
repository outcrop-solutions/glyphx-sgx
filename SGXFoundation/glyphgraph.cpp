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

	GlyphGraph::GlyphGraph(const GlyphGraph& graph) :
		stlplus::ntree<Glyph>(graph) {


	}

	GlyphGraph::~GlyphGraph() {


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