#include "antzobject.h"

namespace SynGlyphXANTz {


	ANTzObject::ANTzObject() :
		m_scale{ { 1.0, 1.0, 1.0 } },
		m_position{ { 0.0, 0.0, 0.0 } },
		m_rotation{ { 0.0, 0.0, 0.0 } }
	{
	}

	ANTzObject::ANTzObject(const ANTzObject& properties) :
		m_scale(properties.m_scale),
		m_position(properties.m_position),
		m_rotation(properties.m_rotation) {


	}

	ANTzObject::~ANTzObject()
	{
	}

	ANTzObject& ANTzObject::operator=(const ANTzObject& properties) {

		m_scale = properties.m_scale;
		m_position = properties.m_position;
		m_rotation = properties.m_rotation;

		return *this;
	}

	bool ANTzObject::operator==(const ANTzObject& properties) const {

		if (m_scale != properties.m_scale) {

			return false;
		}

		if (m_position != properties.m_position) {

			return false;
		}

		if (m_rotation != properties.m_rotation) {

			return false;
		}

		return true;
	}

	bool ANTzObject::operator!=(const ANTzObject& properties) const {

		return !operator==(properties);
	}
	/*
	bool ANTzObject::operator<(const ANTzObject& properties) const {

		//Need a less than operator for the tree class.  Since less than doesn't make sense WRT glyphs, just compare the x position since that is currently used as coordinate for child topology positioning
		return m_position[0] < properties.m_position[0];
	}*/

	void ANTzObject::SetRotation(const SynGlyphX::Vector3& rotation) {

		m_rotation = rotation;
	}

	const SynGlyphX::Vector3& ANTzObject::GetRotation() const {

		return m_rotation;
	}

	void ANTzObject::SetPosition(const SynGlyphX::Vector3& position) {

		m_position = position;
	}

	const SynGlyphX::Vector3& ANTzObject::GetPosition() const {

		return m_position;
	}

	void ANTzObject::SetScale(const SynGlyphX::Vector3& scale) {

		m_scale = scale;
	}

	const SynGlyphX::Vector3& ANTzObject::GetScale() const {

		return m_scale;
	}

	void ANTzObject::SetColor(const SynGlyphX::GlyphColor& color) {

		m_color = color;
	}

	const SynGlyphX::GlyphColor& ANTzObject::GetColor() const {

		return m_color;
	}

} //namespace SynGlyphXANTz
