#include "glyphproperties.h"
#include "data/nptypes.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/lexical_cast.hpp>

namespace SynGlyphXANTz {

	const GlyphProperties::ShapeBimap GlyphProperties::s_shapeNames = boost::assign::list_of < GlyphProperties::ShapeBimap::relation >
		( Shape::Cube, L"Cube" )
		( Shape::Sphere, L"Sphere" )
		( Shape::Cone, L"Cone" )
		( Shape::Torus, L"Torus" )
		( Shape::Dodecahedron, L"Dodecahedron" )
		( Shape::Octahedron, L"Octahedron" )
		( Shape::Tetrahedron, L"Tetrahedron" )
		( Shape::Icosahedron, L"Icosahedron" )
		( Shape::Pin, L"Pin" )
		( Shape::Cylinder, L"Cylinder" );

	const GlyphProperties::SurfaceBimap GlyphProperties::s_surfaceNames = boost::assign::list_of < GlyphProperties::SurfaceBimap::relation >
		( Surface::Wireframe, L"Wireframe" )
		( Surface::Solid, L"Solid" );

	const GlyphProperties::TopologyBimap GlyphProperties::s_topologyNames = boost::assign::list_of < GlyphProperties::TopologyBimap::relation >
		( Topology::Null, L"Euclidean" )
		( Topology::CubePlane, L"Cube" )
		( Topology::SphereNonZeroRadius, L"Sphere" )
		( Topology::Circle, L"Torus" )
		( Topology::CylinderSide, L"Cylinder" )
		( Topology::LinePin, L"Pin" )
		( Topology::LineRod, L"Rod" )
		( Topology::SphereZeroRadius, L"Point" );

    boost::shared_ptr<GlyphProperties> GlyphProperties::s_root = CreateRootPin();
    boost::shared_ptr<GlyphProperties> GlyphProperties::s_template(new GlyphProperties());

    GlyphProperties::GlyphProperties()
        : GlyphMappableProperties(),
        m_tagOffset{ { 0.0, 1.5, 0.0 } },
        m_geometryShape(Shape::Torus),
        m_geometrySurface(Surface::Solid),
        m_topology(Topology::Circle) {

    }

    GlyphProperties::GlyphProperties(pNPnode node) {

        m_tagOffset[0] = node->tagOffset.x;
        m_tagOffset[1] = node->tagOffset.y;
        m_tagOffset[2] = node->tagOffset.z;

        m_scale[0] = node->scale.x;
        m_scale[1] = node->scale.y;
        m_scale[2] = node->scale.z;

        m_position[0] = node->translate.x;
		m_position[1] = node->translate.y;
		m_position[2] = node->translate.z;

        m_rotation[0] = node->rotate.x;
		m_rotation[1] = node->rotate.y;
		m_rotation[2] = node->rotate.z;

        m_color.Set(0, node->color.r);
		m_color.Set(1, node->color.g);
		m_color.Set(2, node->color.b);
		m_color.Set(3, node->color.a);

        m_ratio = node->ratio;

        m_geometryShape = static_cast<Shape>(node->geometry / 2);
        m_geometrySurface = static_cast<Surface>(node->geometry % 2);

        //This is necessary because the enum for geometries in ANTz is screwed up
        if (m_geometryShape == Shape::Pin) {
            m_geometrySurface = static_cast<Surface>(1 - m_geometrySurface);
        }

        m_topology = static_cast<Topology>(node->topo);
    }

    GlyphProperties::GlyphProperties(const GlyphProperties& properties) : 
        GlyphMappableProperties(properties),
        m_tagOffset(properties.m_tagOffset),
        m_geometryShape(properties.m_geometryShape),
        m_geometrySurface(properties.m_geometrySurface),
        m_topology(properties.m_topology) {

    }

	GlyphProperties::GlyphProperties(const SynGlyphX::CSVFileHandler::CSVValues& csvValues)
		: GlyphMappableProperties() {

		m_tagOffset[0] = boost::lexical_cast<double>(csvValues[31]);
		m_tagOffset[1] = boost::lexical_cast<double>(csvValues[32]);
		m_tagOffset[2] = boost::lexical_cast<double>(csvValues[33]);

		m_scale[0] = boost::lexical_cast<double>(csvValues[25]);
		m_scale[1] = boost::lexical_cast<double>(csvValues[26]);
		m_scale[2] = boost::lexical_cast<double>(csvValues[27]);

		m_position[0] = boost::lexical_cast<double>(csvValues[28]);
		m_position[1] = boost::lexical_cast<double>(csvValues[29]);
		m_position[2] = boost::lexical_cast<double>(csvValues[30]);

		m_rotation[0] = boost::lexical_cast<double>(csvValues[37]);
		m_rotation[1] = boost::lexical_cast<double>(csvValues[38]);
		m_rotation[2] = boost::lexical_cast<double>(csvValues[39]);

		m_color.Set(0, boost::lexical_cast<double>(csvValues[55]));
		m_color.Set(1, boost::lexical_cast<double>(csvValues[56]));
		m_color.Set(2, boost::lexical_cast<double>(csvValues[57]));
		m_color.Set(3, boost::lexical_cast<double>(csvValues[58]));

		m_ratio = boost::lexical_cast<double>(csvValues[53]);

		int geometry = boost::lexical_cast<int>(csvValues[50]);
		m_geometryShape = static_cast<Shape>(geometry / 2);
		m_geometrySurface = static_cast<Surface>(geometry % 2);

		//This is necessary because the enum for geometries in ANTz is screwed up
		if (m_geometryShape == Shape::Pin) {
			m_geometrySurface = static_cast<Surface>(1 - m_geometrySurface);
		}

		m_topology = static_cast<Topology>(boost::lexical_cast<int>(csvValues[63]));
	}

    GlyphProperties::~GlyphProperties()
    {
    }

	GlyphProperties& GlyphProperties::operator=(const GlyphProperties& properties) {

		GlyphMappableProperties::operator=(properties);

		m_tagOffset = properties.m_tagOffset;
		m_geometryShape = properties.m_geometryShape;
		m_geometrySurface = properties.m_geometrySurface;
		m_topology = properties.m_topology;

		return *this;
	}

	bool GlyphProperties::operator==(const GlyphProperties& properties) const {

		if (!GlyphMappableProperties::operator==(properties)) {

			return false;
		}
		
		if (m_tagOffset != properties.m_tagOffset) {

			return false;
		}

		if (m_geometryShape != properties.m_geometryShape) {

			return false;
		}

		if (m_geometrySurface != properties.m_geometrySurface) {

			return false;
		}

		if (m_topology != properties.m_topology) {

			return false;
		}

		return true;
	}

	bool GlyphProperties::operator!=(const GlyphProperties& properties) const {

		return !operator==(properties);
	}

    void GlyphProperties::SetGeometry(Shape shape, Surface surface) {
        m_geometryShape = shape;
        m_geometrySurface = surface;
    }

	GlyphProperties::Shape GlyphProperties::GetShape() const {
        return m_geometryShape;
    }

	GlyphProperties::Surface GlyphProperties::GetSurface() const {
        return m_geometrySurface;
    }

    void GlyphProperties::SetTopology(Topology topology) {
        m_topology = topology;
    }

	GlyphProperties::Topology GlyphProperties::GetTopology() const {
        return m_topology;
    }

	GlyphProperties::ConstSharedPtr GlyphProperties::GetRoot() {
        return s_root;
    }

	GlyphProperties::SharedPtr GlyphProperties::CreateRootPin() {
        boost::shared_ptr<GlyphProperties> root(new GlyphProperties());

        root->SetGeometry(Shape::Pin, Surface::Solid);
        root->SetTopology(Topology::LinePin);

        return root;
    }

	GlyphProperties::ConstSharedPtr GlyphProperties::GetTemplate() {
        return s_template;
    }

    void GlyphProperties::SetTagOffset(double x, double y, double z) {
        m_tagOffset[0] = x;
        m_tagOffset[1] = y;
        m_tagOffset[2] = z;
    }

    const SynGlyphX::Vector3& GlyphProperties::GetTagOffset() const {
        return m_tagOffset;
    }

} //namespace SynGlyphXANTz
