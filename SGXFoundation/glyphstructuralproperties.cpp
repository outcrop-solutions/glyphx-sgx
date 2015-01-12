#include "glyphstructuralproperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const GlyphStructuralProperties::ShapeBimap GlyphStructuralProperties::s_shapeNames = boost::assign::list_of < GlyphStructuralProperties::ShapeBimap::relation >
		(GlyphStructuralProperties::Shape::Cube, L"Cube")
		(GlyphStructuralProperties::Shape::Sphere, L"Sphere")
		(GlyphStructuralProperties::Shape::Cone, L"Cone")
		(GlyphStructuralProperties::Shape::Torus, L"Torus")
		(GlyphStructuralProperties::Shape::Dodecahedron, L"Dodecahedron")
		(GlyphStructuralProperties::Shape::Octahedron, L"Octahedron")
		(GlyphStructuralProperties::Shape::Tetrahedron, L"Tetrahedron")
		(GlyphStructuralProperties::Shape::Icosahedron, L"Icosahedron")
		(GlyphStructuralProperties::Shape::Pin, L"Pin")
		(GlyphStructuralProperties::Shape::Cylinder, L"Cylinder");

	const GlyphStructuralProperties::SurfaceBimap GlyphStructuralProperties::s_surfaceNames = boost::assign::list_of < GlyphStructuralProperties::SurfaceBimap::relation >
		(GlyphStructuralProperties::Surface::Wireframe, L"Wireframe")
		(GlyphStructuralProperties::Surface::Solid, L"Solid");

	const GlyphStructuralProperties::VirtualTopologyBimap GlyphStructuralProperties::s_virtualTopologyNames = boost::assign::list_of < GlyphStructuralProperties::VirtualTopologyBimap::relation >
		(GlyphStructuralProperties::VirtualTopology::Null, L"Euclidean")
		(GlyphStructuralProperties::VirtualTopology::CubePlane, L"Cube")
		(GlyphStructuralProperties::VirtualTopology::SphereNonZeroRadius, L"Sphere")
		(GlyphStructuralProperties::VirtualTopology::Circle, L"Torus")
		(GlyphStructuralProperties::VirtualTopology::CylinderSide, L"Cylinder")
		(GlyphStructuralProperties::VirtualTopology::LinePin, L"Pin")
		(GlyphStructuralProperties::VirtualTopology::LineRod, L"Rod")
		(GlyphStructuralProperties::VirtualTopology::SphereZeroRadius, L"Point");

	GlyphStructuralProperties::GlyphStructuralProperties(Shape shape, Surface surface, VirtualTopology virtualTopology) :
		m_geometryShape(shape),
		m_geometrySurface(surface),
		m_virtualTopology(virtualTopology)
	{
	}

	GlyphStructuralProperties::GlyphStructuralProperties(const boost::property_tree::wptree& propertyTree) : 
		m_geometryShape(s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape"))),
		m_geometrySurface(s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface"))),
		m_virtualTopology(s_virtualTopologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology"))) {

	}

	GlyphStructuralProperties::GlyphStructuralProperties(const GlyphStructuralProperties& properties) :
		m_geometryShape(properties.m_geometryShape),
		m_geometrySurface(properties.m_geometrySurface),
		m_virtualTopology(properties.m_virtualTopology) {


	}

	GlyphStructuralProperties::~GlyphStructuralProperties()
	{
	}

	GlyphStructuralProperties& GlyphStructuralProperties::operator=(const GlyphStructuralProperties& properties) {

		m_geometryShape = properties.m_geometryShape;
		m_geometrySurface = properties.m_geometrySurface;
		m_virtualTopology = properties.m_virtualTopology;

		return *this;
	}

	void GlyphStructuralProperties::SetGeometryShape(Shape shape) {

		m_geometryShape = shape;
	}

	GlyphStructuralProperties::Shape GlyphStructuralProperties::GetGeometryShape() const {

		return m_geometryShape;
	}

	void GlyphStructuralProperties::SetGeometrySurface(Surface surface) {

		m_geometrySurface = surface;
	}

	GlyphStructuralProperties::Surface GlyphStructuralProperties::GetGeometrySurface() const {

		return m_geometrySurface;
	}

	void GlyphStructuralProperties::SetVirtualTopology(VirtualTopology virtualTopology) {

		m_virtualTopology = virtualTopology;
	}

	GlyphStructuralProperties::VirtualTopology GlyphStructuralProperties::GetVirtualTopology() const {

		return m_virtualTopology;
	}

	boost::property_tree::wptree& GlyphStructuralProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put<std::wstring>(L"<xmlattr>.Shape", s_shapeNames.left.at(m_geometryShape));
		propertyTree.put<std::wstring>(L"<xmlattr>.Surface", s_surfaceNames.left.at(m_geometrySurface));
		propertyTree.put<std::wstring>(L"<xmlattr>.Topology", s_virtualTopologyNames.left.at(m_virtualTopology));

		return propertyTree;
	}

} //namespace SynGlyphX