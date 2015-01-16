#ifndef SYNGLYPHX_GLYPHTEMPLATE_H
#define SYNGLYPHX_GLYPHTEMPLATE_H

#include "sgxfoundation.h"
#include <array>
#include "glyphstructuralproperties.h"

namespace SynGlyphX {

	template <typename NumericType, typename ColorType, typename TextType>
	class SGXFOUNDATION_API GlyphTemplate {

	public:
		typedef std::array<NumericType, 3> NumericTypeXYZ;

		GlyphTemplate(const GlyphStructuralProperties& properties = GlyphStructuralProperties(GlyphStructuralProperties::Shape::Torus, GlyphStructuralProperties::Surface::Solid, GlyphStructuralProperties::VirtualTopology::Circle)) :
			m_position(NumericTypeXYZ()),
			m_rotation(NumericTypeXYZ()),
			m_scale(NumericTypeXYZ()),
			m_color(ColorType()),
			m_transparency(NumericType()),
			m_tag(TextType()),
			m_description(TextType()),
			m_rotationRate(NumericTypeXYZ()),
			m_structure(properties) {

		}

		GlyphTemplate(const GlyphTemplate& glyph) :
			m_position(glyph.m_position),
			m_rotation(glyph.m_rotation),
			m_scale(glyph.m_scale),
			m_color(glyph.m_color),
			m_transparency(glyph.m_transparency),
			m_tag(glyph.m_tag),
			m_description(glyph.m_description),
			m_rotationRate(glyph.m_rotationRate),
			m_structure(glyph.m_structure) {

		}

		virtual ~GlyphTemplate() {

		}

		GlyphStructuralProperties& GetStructure() {

			return m_structure;
		}

		const GlyphStructuralProperties& GetStructure() const {

			return m_structure;
		}

		NumericTypeXYZ& GetPosition() {

			return m_position;
		}

		const NumericTypeXYZ& GetPosition() const {

			return m_position;
		}

		NumericTypeXYZ& GetRotation() {

			return m_rotation;
		}

		const NumericTypeXYZ& GetRotation() const {

			return m_rotation;
		}

		NumericTypeXYZ& GetScale() {

			return m_scale;
		}

		const NumericTypeXYZ& GetScale() const {

			return m_scale;
		}

		ColorType& GetColor() {

			return m_color;
		}

		const ColorType& GetColor() const {

			return m_color;
		}

		NumericType& GetTransparency() {

			return m_transparency;
		}

		const NumericType& GetTransparency() const {

			return m_transparency;
		}

		TextType& GetTag() {

			return m_tag;
		}

		const TextType& GetTag() const {

			return m_tag;
		}

		TextType& GetDescription() {

			return m_description;
		}

		const TextType& GetDescription() const {

			return m_description;
		}

		NumericTypeXYZ& GetRotationRate() {

			return m_rotationRate;
		}

		const NumericTypeXYZ& GetRotationRate() const {

			return m_rotationRate;
		}

		GlyphTemplate& operator=(const GlyphTemplate& glyph) {

			m_position = glyph.m_position;
			m_rotation = glyph.m_rotation;
			m_scale = glyph.m_scale;
			m_color = glyph.m_color;
			m_transparency = glyph.m_transparency;
			m_tag = glyph.m_tag;
			m_description = glyph.m_description;
			m_rotationRate = glyph.m_rotationRate;
			m_structure = glyph.m_structure;

			return *this;
		}

		bool operator==(const GlyphTemplate& glyph) const {

			if (m_position != glyph.m_position) {

				return false;
			}

			if (m_rotation != glyph.m_rotation) {

				return false;
			}

			if (m_scale != glyph.m_scale) {

				return false;
			}

			if (m_color != glyph.m_color) {

				return false;
			}

			if (m_transparency != glyph.m_transparency) {

				return false;
			}

			if (m_tag != glyph.m_tag) {

				return false;
			}

			if (m_description != glyph.m_description) {

				return false;
			}

			if (m_rotationRate != glyph.m_rotationRate) {

				return false;
			}

			if (m_structure != glyph.m_structure) {

				return false;
			}

			return true;
		}

		bool operator!=(const GlyphTemplate& glyph) const {

			return !operator==(glyph);
		}

	protected:
		//Appearance Properties
		NumericTypeXYZ m_position;
		NumericTypeXYZ m_rotation;
		NumericTypeXYZ m_scale;
		ColorType m_color;
		NumericType m_transparency;

		//Tag & Description Properties
		TextType m_tag;
		TextType m_description;

		//Animation Properties
		NumericTypeXYZ m_rotationRate;

		//Glyph Structure
		GlyphStructuralProperties m_structure;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHTEMPLATE_H