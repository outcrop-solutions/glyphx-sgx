#include "glyph.h"
#include "data/nptypes.h"

namespace SynGlyphX {

    boost::shared_ptr<Glyph> Glyph::s_root = CreateRootPin();
    boost::shared_ptr<Glyph> Glyph::s_template(new Glyph());

    Glyph::Glyph()
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

    Glyph::Glyph(pNPnode node) {

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

    Glyph::~Glyph()
    {
    }

    void Glyph::SetRotate(double x, double y, double z) {
        m_rotate[0] = x;
        m_rotate[1] = y;
        m_rotate[2] = z;
    }

    const Vector3& Glyph::GetRotate() const {
        return m_rotate;
    }

    void Glyph::SetTranslate(double x, double y, double z) {
        m_translate[0] = x;
        m_translate[1] = y;
        m_translate[2] = z;
    }

    const Vector3& Glyph::GetTranslate() const {
        return m_translate;
    }

    void Glyph::SetScale(double x, double y, double z) {
        m_scale[0] = x;
        m_scale[1] = y;
        m_scale[2] = z;
    }

    const Vector3& Glyph::GetScale() const {
        return m_scale;
    }

    void Glyph::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
        m_color[0] = red;
        m_color[1] = green;
        m_color[2] = blue;
        m_color[3] = alpha;
    }

    const Color& Glyph::GetColor() const {
        return m_color;
    }

    void Glyph::SetRatio(double ratio) {
        m_ratio;
    }

    double Glyph::GetRatio() const {
        return m_ratio;
    }
    /*
    void Glyph::AddChild(boost::shared_ptr<Glyph> glyph) {
        m_children.push_back(glyph);
    }

    boost::shared_ptr<Glyph> Glyph::GetChild(unsigned int index) const {
        return m_children[index];
    }*/

    void Glyph::SetGeometry(Geometry::Shape shape, Geometry::Surface surface) {
        m_geometryShape = shape;
        m_geometrySurface = surface;
    }

    Geometry::Shape Glyph::GetShape() const {
        return m_geometryShape;
    }

    Geometry::Surface Glyph::GetSurface() const {
        return m_geometrySurface;
    }

    void Glyph::SetTopology(Topology::Type topology) {
        m_topology = topology;
    }

    Topology::Type Glyph::GetTopology() const {
        return m_topology;
    }
    
    boost::shared_ptr<Glyph> Glyph::GetRoot() {
        return s_root;
    }

    boost::shared_ptr<Glyph> Glyph::CreateRootPin() {
        boost::shared_ptr<Glyph> root(new Glyph());

        root->SetGeometry(Geometry::Pin, Geometry::Solid);
        root->SetTopology(Topology::Pin);

        return root;
    }

    boost::shared_ptr<const Glyph> Glyph::GetTemplate() {
        return s_template;
    }
    /*
    void Glyph::ClearChildren() {
        m_children.clear();
    }*/

    unsigned int Glyph::GetNumberOfChildren() const {
        return m_numberOfChildren;
    }
    
    void Glyph::SetNumberOfChildren(unsigned int numberOfChildren) {
        m_numberOfChildren = numberOfChildren;
    }

    void Glyph::SetTagOffset(double x, double y, double z) {
        m_tagOffset[0] = x;
        m_tagOffset[1] = y;
        m_tagOffset[2] = z;
    }

    const Vector3& Glyph::GetTagOffset() const {
        return m_tagOffset;
    }

} //namespace SynGlyphX