#ifndef SYNGLYPHX_GLYPH_H
#define SYNGLYPHX_GLYPH_H

#include "sgxfoundation.h"
#include "glyphtemplate.h"
#include "glyphcolor.h"
#include "foundationtypes.h"

namespace SynGlyphX {

	class SGXFOUNDATION_API Glyph : public GlyphTemplate < double, GlyphColor, std::wstring >
	{
	public:
		Glyph();
		Glyph(const Glyph& glyph);
		virtual ~Glyph();

		Vector3& GetTagOffset();
		const Vector3& GetTagOffset() const;

		Glyph& operator=(const Glyph& glyph);
		bool operator==(const Glyph& glyph) const;
		bool operator!=(const Glyph& glyph) const;

		static const Glyph s_defaultGlyph;
		static const Glyph s_defaultRootGlyph;

	protected:
		static Glyph CreateDefaultGlyph(GlyphStructuralProperties::Shape geometryShape, GlyphStructuralProperties::VirtualTopology virtualTopology);

		Vector3 m_tagOffset;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPH_H