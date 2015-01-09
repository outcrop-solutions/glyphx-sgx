#include "glyphgraph.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	Glyph::Glyph() :
		scale{ { 1.0, 1.0, 1.0 } },
		position{ { 0.0, 0.0, 0.0 } },
		rotation{ { 0.0, 0.0, 0.0 } },
		color({ { 50, 101, 101, 255 } }),
		geometryShape(Shape::Torus),
		geometrySurface(Surface::Solid),
		virtualTopology(VirtualTopology::Circle),
		tag(L""),
		description(L"")
	{
	}

	Glyph::Glyph(const Glyph& glyph) :
		scale(glyph.scale),
		position(glyph.position),
		rotation(glyph.rotation),
		color(glyph.color),
		geometryShape(glyph.geometryShape),
		geometrySurface(glyph.geometrySurface),
		virtualTopology(glyph.virtualTopology),
		tag(glyph.tag),
		description(glyph.description) {

	}

	Glyph::~Glyph()
	{
	}

	GlyphEdge::GlyphEdge() : 
		m_type(Type::Topological) {

	}

	GlyphEdge::GlyphEdge(const GlyphEdge& edge) :
		m_type(edge.m_type) {

	}

	GlyphEdge::~GlyphEdge() {

	}

	GlyphGraph::GlyphGraph() :
		boost::undirected_graph < Glyph, GlyphEdge >() {


	}

	GlyphGraph::GlyphGraph(const GlyphGraph& graph) :
		boost::undirected_graph < Glyph, GlyphEdge >(graph) {


	}

	GlyphGraph::~GlyphGraph() {


	}

	const GlyphGraph::ShapeBimap GlyphGraph::s_shapeNames = boost::assign::list_of < GlyphGraph::ShapeBimap::relation >
		(Glyph::Shape::Cube, L"Cube")
		(Glyph::Shape::Sphere, L"Sphere")
		(Glyph::Shape::Cone, L"Cone")
		(Glyph::Shape::Torus, L"Torus")
		(Glyph::Shape::Dodecahedron, L"Dodecahedron")
		(Glyph::Shape::Octahedron, L"Octahedron")
		(Glyph::Shape::Tetrahedron, L"Tetrahedron")
		(Glyph::Shape::Icosahedron, L"Icosahedron")
		(Glyph::Shape::Pin, L"Pin")
		(Glyph::Shape::Cylinder, L"Cylinder");

	const GlyphGraph::SurfaceBimap GlyphGraph::s_surfaceNames = boost::assign::list_of < GlyphGraph::SurfaceBimap::relation >
		(Glyph::Surface::Wireframe, L"Wireframe")
		(Glyph::Surface::Solid, L"Solid");

	const GlyphGraph::VirtualTopologyBimap GlyphGraph::s_virtualTopologyNames = boost::assign::list_of < GlyphGraph::VirtualTopologyBimap::relation >
		(Glyph::VirtualTopology::Null, L"Euclidean")
		(Glyph::VirtualTopology::CubePlane, L"Cube")
		(Glyph::VirtualTopology::SphereNonZeroRadius, L"Sphere")
		(Glyph::VirtualTopology::Circle, L"Torus")
		(Glyph::VirtualTopology::CylinderSide, L"Cylinder")
		(Glyph::VirtualTopology::LinePin, L"Pin")
		(Glyph::VirtualTopology::LineRod, L"Rod")
		(Glyph::VirtualTopology::SphereZeroRadius, L"Point");

} //namespace SynGlyphX