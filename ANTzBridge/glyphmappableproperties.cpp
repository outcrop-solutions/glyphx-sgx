#include "glyphmappableproperties.h"

namespace SynGlyphX {

	GlyphMappableProperties GlyphMappableProperties::s_zero = CreatePropertiesZero();

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

	GlyphMappableProperties& GlyphMappableProperties::operator=(const GlyphMappableProperties& properties) {

		m_scale = properties.m_scale;
		m_translate = properties.m_translate;
		m_rotate = properties.m_rotate;
		m_color = properties.m_color;
		m_ratio = properties.m_ratio;

		return *this;
	}

	bool GlyphMappableProperties::operator==(const GlyphMappableProperties& properties) const {

		if (m_scale != properties.m_scale) {

			return false;
		}

		if (m_translate != properties.m_translate) {

			return false;
		}

		if (m_rotate != properties.m_rotate) {

			return false;
		}

		if (m_color != properties.m_color) {

			return false;
		}

		if (m_ratio != properties.m_ratio) {

			return false;
		}

		return true;
	}

	bool GlyphMappableProperties::operator<(const GlyphMappableProperties& properties) const {

		//Need a less than operator for the tree class.  Since less than doesn't make sense WRT glyphs, just compare the x position since that is currently used as coordinate for child topology positioning
		return m_translate[0] < properties.m_translate[0];
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

	const GlyphMappableProperties& GlyphMappableProperties::GetPropertiesZero() {

		return s_zero;
	}

	GlyphMappableProperties GlyphMappableProperties::CreatePropertiesZero() {

		GlyphMappableProperties properties;

		properties.SetScale(0.0, 0.0, 0.0);
		properties.SetRatio(0.0);
		properties.SetColor(0, 0, 0, 0);

		return properties;
	}

} //namespace SynGlyphX
