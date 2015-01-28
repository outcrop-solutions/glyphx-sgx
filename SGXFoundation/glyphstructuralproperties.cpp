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
		//(GlyphStructuralProperties::VirtualTopology::Null, L"Euclidean")
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
		m_virtualTopology(virtualTopology),
		m_torusRatio(0.1)
	{
	}

	GlyphStructuralProperties::GlyphStructuralProperties(const boost::property_tree::wptree& propertyTree) : 
		m_geometryShape(s_shapeNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Shape"))),
		m_geometrySurface(s_surfaceNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Surface"))),
		m_virtualTopology(s_virtualTopologyNames.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.Topology"))),
		m_torusRatio(propertyTree.get_optional<double>(L"Ratio").get_value_or(0.1)) {

	}

	GlyphStructuralProperties::GlyphStructuralProperties(const GlyphStructuralProperties& properties) :
		m_geometryShape(properties.m_geometryShape),
		m_geometrySurface(properties.m_geometrySurface),
		m_virtualTopology(properties.m_virtualTopology),
		m_torusRatio(properties.m_torusRatio) {


	}

	GlyphStructuralProperties::~GlyphStructuralProperties()
	{
	}

	GlyphStructuralProperties& GlyphStructuralProperties::operator=(const GlyphStructuralProperties& properties) {

		m_geometryShape = properties.m_geometryShape;
		m_geometrySurface = properties.m_geometrySurface;
		m_virtualTopology = properties.m_virtualTopology;
		m_torusRatio = properties.m_torusRatio;

		return *this;
	}

	bool GlyphStructuralProperties::operator==(const GlyphStructuralProperties& properties) const {

		if (m_geometryShape != properties.m_geometryShape) {

			return false;
		}

		if (m_geometrySurface != properties.m_geometrySurface) {

			return false;
		}

		if (m_virtualTopology != properties.m_virtualTopology) {

			return false;
		}

		if (m_torusRatio != properties.m_torusRatio) {

			return false;
		}

		return true;
	}

	bool GlyphStructuralProperties::operator!=(const GlyphStructuralProperties& properties) const {

		return !operator==(properties);
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

	void GlyphStructuralProperties::SetTorusRatio(double ratio) {

		m_torusRatio = ratio;
	}

	double GlyphStructuralProperties::GetTorusRatio() const {

		return m_torusRatio;
	}

	boost::property_tree::wptree& GlyphStructuralProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put<std::wstring>(L"<xmlattr>.Shape", s_shapeNames.left.at(m_geometryShape));
		propertyTree.put<std::wstring>(L"<xmlattr>.Surface", s_surfaceNames.left.at(m_geometrySurface));
		propertyTree.put<std::wstring>(L"<xmlattr>.Topology", s_virtualTopologyNames.left.at(m_virtualTopology));
		propertyTree.put<double>(L"Ratio", m_torusRatio);

		return propertyTree;
	}

} //namespace SynGlyphX