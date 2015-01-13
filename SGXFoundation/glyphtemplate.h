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

		NumericTypeXYZ position;
		NumericTypeXYZ rotation;
		NumericTypeXYZ scale;
		ColorType color;
		NumericType transparency;
		TextType tag;
		TextType description;

		GlyphStructuralProperties structure;

		GlyphTemplate(const GlyphStructuralProperties& properties = GlyphStructuralProperties(Shape::Torus, Surface::Solid, VirtualTopology::Circle)) :
			position(NumericTypeXYZ()),
			rotation(NumericTypeXYZ()),
			scale(NumericTypeXYZ()),
			color(ColorType()),
			transparency(NumericType()),
			tag(TextType()),
			description(TextType()),
			structure(properties) {

		}

		GlyphTemplate(const GlyphTemplate& glyph) :
			position(glyph.position),
			rotation(glyph.rotation),
			scale(glyph.scale),
			color(glyph.color),
			transparency(glyph.transparency),
			tag(glyph.tag),
			description(glyph.description),
			structure(glyph.structure) {

		}

		virtual ~GlyphTemplate() {

		}
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHTEMPLATE_H