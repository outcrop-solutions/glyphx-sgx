#include "glyph.h"

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

    void Glyph::AddChild(boost::shared_ptr<Glyph> glyph) {
        m_children.push_back(glyph);
    }

    boost::shared_ptr<Glyph> Glyph::GetChild(unsigned int index) {
        return m_children[index];
    }

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

    void Glyph::ClearChildren() {
        m_children.clear();
    }

} //namespace SynGlyphX