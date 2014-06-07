#include "glyphproperties.h"

namespace SynGlyphX {

    boost::shared_ptr<GlyphProperties> GlyphProperties::s_root = CreateRootPin();
    boost::shared_ptr<GlyphProperties> GlyphProperties::s_template(new GlyphProperties());

    GlyphProperties::GlyphProperties()
        : m_tagOffset{ { 0.0, 1.5, 0.0 } },
        m_scale{ { 1.0, 1.0, 1.0 } },
        m_translate{ { 0.0, 0.0, 0.0 } },
        m_rotate{ { 0.0, 0.0, 0.0 } },
        m_color{ { 50, 101, 101, 255 } },
        m_ratio(0.1),
        m_geometryShape(Geometry::Torus),
        m_geometrySurface(Geometry::Solid),
        m_topology(Topology::Torus) {

    }

    GlyphProperties::GlyphProperties(pNPnode node) {

        m_tagOffset[0] = node->tagOffset.x;
        m_tagOffset[1] = node->tagOffset.y;
        m_tagOffset[2] = node->tagOffset.z;

        m_scale[0] = node->scale.x;
        m_scale[1] = node->scale.y;
        m_scale[2] = node->scale.z;

        m_translate[0] = node->translate.x;
        m_translate[1] = node->translate.y;
        m_translate[2] = node->translate.z;

        m_rotate[0] = node->rotate.x;
        m_rotate[1] = node->rotate.y;
        m_rotate[2] = node->rotate.z;

        m_color[0] = node->color.r;
        m_color[1] = node->color.g;
        m_color[2] = node->color.b;
        m_color[3] = node->color.a;

        m_ratio = node->ratio;

        m_geometryShape = static_cast<Geometry::Shape>(node->geometry / 2);
        m_geometrySurface = static_cast<Geometry::Surface>(node->geometry % 2);

        //This is necessary because the enum for geometries in ANTz is screwed up
        if (m_geometryShape == Geometry::Pin) {
            m_geometrySurface = static_cast<Geometry::Surface>(1 - m_geometrySurface);
        }

        m_topology = static_cast<Topology::Type>(node->topo);
    }

    GlyphProperties::GlyphProperties(const GlyphProperties& properties) : 
        m_tagOffset(properties.m_tagOffset),
        m_scale(properties.m_scale),
        m_translate(properties.m_translate),
        m_rotate(properties.m_rotate),
        m_color(properties.m_color),
        m_ratio(properties.m_ratio),
        m_geometryShape(properties.m_geometryShape),
        m_geometrySurface(properties.m_geometrySurface),
        m_topology(properties.m_topology) {

    }

    GlyphProperties::~GlyphProperties()
    {
    }

    void GlyphProperties::SetRotate(double x, double y, double z) {
        m_rotate[0] = x;
        m_rotate[1] = y;
        m_rotate[2] = z;
    }

    const Vector3& GlyphProperties::GetRotate() const {
        return m_rotate;
    }

    void GlyphProperties::SetTranslate(double x, double y, double z) {
        m_translate[0] = x;
        m_translate[1] = y;
        m_translate[2] = z;
    }

    const Vector3& GlyphProperties::GetTranslate() const {
        return m_translate;
    }

    void GlyphProperties::SetScale(double x, double y, double z) {
        m_scale[0] = x;
        m_scale[1] = y;
        m_scale[2] = z;
    }

    const Vector3& GlyphProperties::GetScale() const {
        return m_scale;
    }

    void GlyphProperties::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
        m_color[0] = red;
        m_color[1] = green;
        m_color[2] = blue;
        m_color[3] = alpha;
    }

    const Color& GlyphProperties::GetColor() const {
        return m_color;
    }

    void GlyphProperties::SetRatio(double ratio) {
        m_ratio;
    }

    double GlyphProperties::GetRatio() const {
        return m_ratio;
    }

    void GlyphProperties::SetGeometry(Geometry::Shape shape, Geometry::Surface surface) {
        m_geometryShape = shape;
        m_geometrySurface = surface;
    }

    Geometry::Shape GlyphProperties::GetShape() const {
        return m_geometryShape;
    }

    Geometry::Surface GlyphProperties::GetSurface() const {
        return m_geometrySurface;
    }

    void GlyphProperties::SetTopology(Topology::Type topology) {
        m_topology = topology;
    }

    Topology::Type GlyphProperties::GetTopology() const {
        return m_topology;
    }

    boost::shared_ptr<GlyphProperties> GlyphProperties::GetRoot() {
        return s_root;
    }

    boost::shared_ptr<GlyphProperties> GlyphProperties::CreateRootPin() {
        boost::shared_ptr<GlyphProperties> root(new GlyphProperties());

        root->SetGeometry(Geometry::Pin, Geometry::Solid);
        root->SetTopology(Topology::Pin);

        return root;
    }

    boost::shared_ptr<const GlyphProperties> GlyphProperties::GetTemplate() {
        return s_template;
    }

    unsigned int GlyphProperties::GetNumberOfChildren() const {
        return 0;
    }

    void GlyphProperties::SetTagOffset(double x, double y, double z) {
        m_tagOffset[0] = x;
        m_tagOffset[1] = y;
        m_tagOffset[2] = z;
    }

    const Vector3& GlyphProperties::GetTagOffset() const {
        return m_tagOffset;
    }

	std::wstring GlyphProperties::ShapeToString(Geometry::Shape shape) {

		if (shape == SynGlyphX::Geometry::Cube) {
			return L"Cube";
		}
		else if (shape == SynGlyphX::Geometry::Sphere) {
			return L"Sphere";
		}
		else if (shape == SynGlyphX::Geometry::Cone) {
			return L"Cone";
		}
		else if (shape == SynGlyphX::Geometry::Torus) {
			return L"Torus";
		}
		else if (shape == SynGlyphX::Geometry::Dodecahedron) {
			return L"Dodecahedron";
		}
		else if (shape == SynGlyphX::Geometry::Octahedron) {
			return L"Octahedron";
		}
		else if (shape == SynGlyphX::Geometry::Tetrahedron) {
			return L"Tetrahedron";
		}
		else if (shape == SynGlyphX::Geometry::Icosahedron) {
			return L"Icosahedron";
		}
		else if (shape == SynGlyphX::Geometry::Pin) {
			return L"Pin";
		}
		else if (shape == SynGlyphX::Geometry::Cylinder) {
			return L"Cylinder";
		}

		return L"";
	}

	std::wstring GlyphProperties::SurfaceToString(Geometry::Surface surface) {

		if (surface == SynGlyphX::Geometry::Wireframe) {
			return L"Wireframe";
		}
		else if (surface == SynGlyphX::Geometry::Solid) {
			return L"Solid";
		}

		return L"";
	}

	std::wstring GlyphProperties::TopologyTypeToString(Topology::Type topo) {

		if (topo == SynGlyphX::Topology::Null) {
			return L"Euclidean";
		}
		else if (topo == SynGlyphX::Topology::Cube) {
			return L"Cube";
		}
		else if (topo == SynGlyphX::Topology::Sphere) {
			return L"Sphere";
		}
		else if (topo == SynGlyphX::Topology::Torus) {
			return L"Torus";
		}
		else if (topo == SynGlyphX::Topology::Cylinder) {
			return L"Cylinder";
		}
		else if (topo == SynGlyphX::Topology::Pin) {
			return L"Pin";
		}
		else if (topo == SynGlyphX::Topology::Rod) {
			return L"Rod";
		}
		else if (topo == SynGlyphX::Topology::Point) {
			return L"Point";
		}

		return L"";
	}

} //namespace SynGlyphX