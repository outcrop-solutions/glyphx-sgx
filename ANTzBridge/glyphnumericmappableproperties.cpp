#include "glyphnumericmappableproperties.h"

namespace SynGlyphX {

	GlyphNumericMappableProperties GlyphNumericMappableProperties::s_defaultMinProperties = CreateDefaultMinProperties();

	GlyphNumericMappableProperties::GlyphNumericMappableProperties() :
		m_scale{ { 1.0, 1.0, 1.0 } },
		m_position{ { 0.0, 0.0, 0.0 } },
		m_rotation{ { 0.0, 0.0, 0.0 } },
		m_color({ { 50, 101, 101, 255 } }),
		m_ratio(0.1)
	{
	}

	GlyphNumericMappableProperties::GlyphNumericMappableProperties(const GlyphNumericMappableProperties& properties) :
		m_scale(properties.m_scale),
		m_position(properties.m_position),
		m_rotation(properties.m_rotation),
		m_color(properties.m_color),
		m_ratio(properties.m_ratio) {


	}

	GlyphNumericMappableProperties::~GlyphNumericMappableProperties()
	{
	}

	GlyphNumericMappableProperties& GlyphNumericMappableProperties::operator=(const GlyphNumericMappableProperties& properties) {

		m_scale = properties.m_scale;
		m_position = properties.m_position;
		m_rotation = properties.m_rotation;
		m_color = properties.m_color;
		m_ratio = properties.m_ratio;

		return *this;
	}

	bool GlyphNumericMappableProperties::operator==(const GlyphNumericMappableProperties& properties) const {

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

		return true;
	}

	bool GlyphNumericMappableProperties::operator!=(const GlyphNumericMappableProperties& properties) const {

		return !operator==(properties);
	}

	bool GlyphNumericMappableProperties::operator<(const GlyphNumericMappableProperties& properties) const {

		//Need a less than operator for the tree class.  Since less than doesn't make sense WRT glyphs, just compare the x position since that is currently used as coordinate for child topology positioning
		return m_position[0] < properties.m_position[0];
	}

	void GlyphNumericMappableProperties::SetRotation(const Vector3& rotation) {

		m_rotation = rotation;
	}

	const Vector3& GlyphNumericMappableProperties::GetRotation() const {

		return m_rotation;
	}

	void GlyphNumericMappableProperties::SetPosition(const Vector3& position) {

		m_position = position;
	}

	const Vector3& GlyphNumericMappableProperties::GetPosition() const {

		return m_position;
	}

	void GlyphNumericMappableProperties::SetScale(const Vector3& scale) {

		m_scale = scale;
	}

	const Vector3& GlyphNumericMappableProperties::GetScale() const {

		return m_scale;
	}

	void GlyphNumericMappableProperties::SetColor(const Color& color) {

		m_color = color;
	}

	const Color& GlyphNumericMappableProperties::GetColor() const {

		return m_color;
	}

	void GlyphNumericMappableProperties::SetRatio(double ratio) {

		m_ratio = ratio;
	}

	double GlyphNumericMappableProperties::GetRatio() const {

		return m_ratio;
	}

	void GlyphNumericMappableProperties::SetToZero() {

		SetScale({ { 0.0, 0.0, 0.0 } });
		SetRatio(0.0);
		SetColor(Color({ { 0, 0, 0, 0 } }));
		SetPosition({ { 0.0, 0.0, 0.0 } });
		SetRotation({ { 0.0, 0.0, 0.0 } });
	}

	void GlyphNumericMappableProperties::AddDifference(const GlyphNumericMappableProperties& properties) {

		m_position[0] += properties.m_position[0];
		m_position[1] += properties.m_position[1];
		m_position[2] += properties.m_position[2];

		m_rotation[0] += properties.m_rotation[0];
		m_rotation[1] += properties.m_rotation[1];
		m_rotation[2] += properties.m_rotation[2];

		m_scale[0] += properties.m_scale[0];
		m_scale[1] += properties.m_scale[1];
		m_scale[2] += properties.m_scale[2];

		m_ratio += properties.m_ratio;

		m_color += properties.m_color;
	}

	void GlyphNumericMappableProperties::Subtract(const GlyphNumericMappableProperties& properties) {

		m_position[0] -= properties.m_position[0];
		m_position[1] -= properties.m_position[1];
		m_position[2] -= properties.m_position[2];

		m_rotation[0] -= properties.m_rotation[0];
		m_rotation[1] -= properties.m_rotation[1];
		m_rotation[2] -= properties.m_rotation[2];

		m_scale[0] -= properties.m_scale[0];
		m_scale[1] -= properties.m_scale[1];
		m_scale[2] -= properties.m_scale[2];

		m_ratio -= properties.m_ratio;

		m_color -= properties.m_color;
	}

	const GlyphNumericMappableProperties& GlyphNumericMappableProperties::GetDefaultMinProperties() {

		return s_defaultMinProperties;
	}

	GlyphNumericMappableProperties GlyphNumericMappableProperties::CreateDefaultMinProperties() {

		GlyphNumericMappableProperties properties;

		properties.SetScale({ { 0.1, 0.1, 0.1 } });
		properties.SetRatio(0.01);
		properties.SetColor(Color({ { 0, 0, 0, 0 } }));
		properties.SetPosition({ { 0.0, 0.0, 0.0 } });
		properties.SetRotation({ { -180.0, -180.0, -180.0 } });

		return properties;
	}

} //namespace SynGlyphX