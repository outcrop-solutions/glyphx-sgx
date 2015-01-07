#include "glyphmappableproperties.h"

namespace SynGlyphXANTz {

    GlyphMappableProperties::GlyphMappableProperties() :
		GlyphNumericMappableProperties(),
		m_tag(L"")
    {
    }

    GlyphMappableProperties::GlyphMappableProperties(const GlyphMappableProperties& properties) :
        GlyphNumericMappableProperties(properties),
		m_tag(properties.m_tag) {

    }

    GlyphMappableProperties::~GlyphMappableProperties()
    {
    }

	GlyphMappableProperties& GlyphMappableProperties::operator=(const GlyphMappableProperties& properties) {

		GlyphNumericMappableProperties::operator=(properties);
		m_tag = properties.m_tag;

		return *this;
	}

	bool GlyphMappableProperties::operator==(const GlyphMappableProperties& properties) const {

		if (GlyphNumericMappableProperties::operator!=(properties)) {

			return false;
		}

		if (m_tag != properties.m_tag) {

			return false;
		}

		return true;
	}

	bool GlyphMappableProperties::operator!=(const GlyphMappableProperties& properties) const {

		return !operator==(properties);
	}

	void GlyphMappableProperties::SetTag(const std::wstring& tag) {

		m_tag = tag;
	}

	const std::wstring& GlyphMappableProperties::GetTag() const {

		return m_tag;
	}

} //namespace SynGlyphXANTz
