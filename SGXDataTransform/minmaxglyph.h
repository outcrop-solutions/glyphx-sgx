#ifndef SYNGLYPHX_MINMAXGLYPH_H
#define SYNGLYPHX_MINMAXGLYPH_H

#include "glyphproperties.h"
#include "glyph.h"
#include <vector>
#include <tree.hh>

namespace SynGlyphX {

	class MinMaxGlyph
	{
	public:
		MinMaxGlyph();
		~MinMaxGlyph();

		const GlyphProperties& GetMinGlyph() const;
		const GlyphProperties& GetMaxGlyph() const;

		void SetMinGlyph(const GlyphProperties& glyph);
		void SetMaxGlyph(const GlyphProperties& glyph);

	private:
		GlyphProperties m_min;
		GlyphProperties m_max;
	};

	typedef tree<MinMaxGlyph> MinMaxGlyphTree;

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPH_H