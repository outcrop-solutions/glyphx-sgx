#include "glyphmappableproperties.h"

namespace SynGlyphX {

    GlyphMappableProperties::GlyphMappableProperties() :
        m_scale{ { 1.0, 1.0, 1.0 } },
        m_translate{ { 0.0, 0.0, 0.0 } },
        m_rotate{ { 0.0, 0.0, 0.0 } },
        m_color{ { 50, 101, 101, 255 } },
        m_ratio(0.1)
    {
    }

    GlyphMappableProperties::GlyphMappableProperties(const GlyphMappableProperties& properties) :
        m_scale(properties.m_scale),
        m_translate(properties.m_translate),
        m_rotate(properties.m_rotate),
        m_color(properties.m_color),
        m_ratio(properties.m_ratio) {

    }

    GlyphMappableProperties::~GlyphMappableProperties()
    {
    }

    void GlyphMappableProperties::SetRotate(double x, double y, double z) {
        
        m_rotate[0] = x;
        m_rotate[1] = y;
        m_rotate[2] = z;
    }

    const Vector3& GlyphMappableProperties::GetRotate() const {
        
        return m_rotate;
    }

    void GlyphMappableProperties::SetTranslate(double x, double y, double z) {
        
        m_translate[0] = x;
        m_translate[1] = y;
        m_translate[2] = z;
    }

    const Vector3& GlyphMappableProperties::GetTranslate() const {
        
        return m_translate;
    }

    void GlyphMappableProperties::SetScale(double x, double y, double z) {
        
        m_scale[0] = x;
        m_scale[1] = y;
        m_scale[2] = z;
    }

    const Vector3& GlyphMappableProperties::GetScale() const {
        
        return m_scale;
    }

    void GlyphMappableProperties::SetColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha) {
        
        m_color[0] = red;
        m_color[1] = green;
        m_color[2] = blue;
        m_color[3] = alpha;
    }

    const Color& GlyphMappableProperties::GetColor() const {
        
        return m_color;
    }

    void GlyphMappableProperties::SetRatio(double ratio) {
        
        m_ratio = ratio;
    }

    double GlyphMappableProperties::GetRatio() const {
        
        return m_ratio;
    }

    unsigned int GlyphMappableProperties::GetNumberOfChildren() const {
        
        return 0;
    }

} //namespace SynGlyphX
