#ifndef SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H
#define SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H

#include "ANTzBridge.h"
#include "foundationtypes.h"
#include "glyphcolor.h"

namespace SynGlyphXANTz {

	class ANTZBRIDGE_API ANTzObject
	{
	public:
		ANTzObject();
		ANTzObject(const ANTzObject& properties);
		virtual ~ANTzObject();

		ANTzObject& operator=(const ANTzObject& properties);
		bool operator==(const ANTzObject& properties) const;
		bool operator!=(const ANTzObject& properties) const;

		void SetRotation(const SynGlyphX::Vector3& rotation);
		const SynGlyphX::Vector3& GetRotation() const;
		void SetPosition(const SynGlyphX::Vector3& position);
		const SynGlyphX::Vector3& GetPosition() const;
		void SetScale(const SynGlyphX::Vector3& scale);
		const SynGlyphX::Vector3& GetScale() const;
		void SetColor(const SynGlyphX::GlyphColor& color);
		const SynGlyphX::GlyphColor& GetColor() const;

	protected:
		SynGlyphX::Vector3 m_rotation;
		SynGlyphX::Vector3 m_scale;
		SynGlyphX::Vector3 m_position;
		SynGlyphX::GlyphColor m_color;
	};

} //namespace SynGlyphXANTz

#endif //SYNGLYPHX_GLYPHNUMERICMAPPABLEPROPERTIES_H