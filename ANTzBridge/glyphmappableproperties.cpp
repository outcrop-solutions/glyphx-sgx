#include "glyphmappableproperties.h"

namespace SynGlyphX {

	GlyphMappableProperties GlyphMappableProperties::s_zero = CreatePropertiesZero();

    GlyphMappableProperties::GlyphMappableProperties() :
        m_scale{ { 1.0, 1.0, 1.0 } },
        m_position{ { 0.0, 0.0, 0.0 } },
		m_rotation{ { 0.0, 0.0, 0.0 } },
        m_color({ { 50, 101, 101, 255 } }),
        m_ratio(0.1),
		m_tag(L"")
    {
    }

    GlyphMappableProperties::GlyphMappableProperties(const GlyphMappableProperties& properties) :
        m_scale(properties.m_scale),
        m_position(properties.m_position),
		m_rotation(properties.m_rotation),
        m_color(properties.m_color),
        m_ratio(properties.m_ratio),
		m_tag(properties.m_tag) {

    }

    GlyphMappableProperties::~GlyphMappableProperties()
    {
    }

	GlyphMappableProperties& GlyphMappableProperties::operator=(const GlyphMappableProperties& properties) {

		m_scale = properties.m_scale;
		m_position = properties.m_position;
		m_rotation = properties.m_rotation;
		m_color = properties.m_color;
		m_ratio = properties.m_ratio;
		m_tag = properties.m_tag;

		return *this;
	}

	bool GlyphMappableProperties::operator==(const GlyphMappableProperties& properties) const {

		if (m_scale != properties.m_scale) {

			return false;
		}

		if (m_position != properties.m_position) {

			return false;
		}

		if (m_rotation != properties.m_rotation) {

			return false;
		}

		if (m_color != properties.m_color) {

			return false;
		}

		if (m_ratio != properties.m_ratio) {

			return false;
		}

		if (m_tag != properties.m_tag) {

			return false;
		}

		return true;
	}

	bool GlyphMappableProperties::operator<(const GlyphMappableProperties& properties) const {

		//Need a less than operator for the tree class.  Since less than doesn't make sense WRT glyphs, just compare the x position since that is currently used as coordinate for child topology positioning
		return m_position[0] < properties.m_position[0];
	}

	void GlyphMappableProperties::SetRotation(const Vector3& rotation) {
        
		m_rotation = rotation;
    }

    const Vector3& GlyphMappableProperties::GetRotation() const {
        
		return m_rotation;
    }

    void GlyphMappableProperties::SetPosition(const Vector3& position) {
        
		m_position = position;
    }

    const Vector3& GlyphMappableProperties::GetPosition() const {
        
        return m_position;
    }

    void GlyphMappableProperties::SetScale(const Vector3& scale) {
        
		m_scale = scale;
    }

    const Vector3& GlyphMappableProperties::GetScale() const {
        
        return m_scale;
    }

    void GlyphMappableProperties::SetColor(const Color& color) {
        
		m_color = color;
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

	void GlyphMappableProperties::SetTag(const std::wstring& tag) {

		m_tag = tag;
	}

	const std::wstring& GlyphMappableProperties::GetTag() const {

		return m_tag;
	}

	const GlyphMappableProperties& GlyphMappableProperties::GetPropertiesZero() {

		return s_zero;
	}

	GlyphMappableProperties GlyphMappableProperties::CreatePropertiesZero() {

		GlyphMappableProperties properties;

		properties.SetScale({ { 0.0, 0.0, 0.0 } });
		properties.SetRatio(0.0);
		properties.SetColor(Color( { { 0, 0, 0, 0 } } ));

		return properties;
	}

} //namespace SynGlyphX
