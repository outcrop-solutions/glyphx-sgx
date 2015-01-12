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
		boost::undirected_graph < Glyph, GlyphEdge >() {


	}

	GlyphGraph::GlyphGraph(const GlyphGraph& graph) :
		boost::undirected_graph < Glyph, GlyphEdge >(graph) {


	}

	GlyphGraph::~GlyphGraph() {


	}

	Glyph GlyphGraph::CreateDefaultGlyph(GlyphStructuralProperties::Shape geometryShape, GlyphStructuralProperties::VirtualTopology virtualTopology) {

		Glyph glyph;
		
		glyph.position = { { 0.0, 0.0, 0.0 } };
		glyph.rotation = { { 0.0, 0.0, 0.0 } };
		glyph.scale = { { 1.0, 1.0, 1.0 } };
		glyph.color = Color({ { 50, 101, 101, 255 } });
		glyph.transparency = 255;
		glyph.structure.SetGeometryShape(geometryShape);
		glyph.structure.SetGeometrySurface(GlyphStructuralProperties::Surface::Solid);
		glyph.structure.SetVirtualTopology(virtualTopology);
		glyph.tag = L"";
		glyph.description = L"";

		return glyph;
	}

} //namespace SynGlyphX