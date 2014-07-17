#ifndef SYNGLYPHX_MINMAXGLYPH_H
#define SYNGLYPHX_MINMAXGLYPH_H

#include "glyphproperties.h"
#include "glyph.h"
#include <vector>
#include <containers/ntree.hpp>

namespace SynGlyphX {

	class MinMaxGlyph
	{
	public:
		MinMaxGlyph(const GlyphProperties& minGlyph);
		~MinMaxGlyph();

		const GlyphProperties& GetMinGlyph() const;
		const GlyphMappableProperties& GetDifference() const;

		void SetMinGlyphProperties(const GlyphMappableProperties& glyph);
		void SetDifference(const GlyphMappableProperties& difference);

	private:
		GlyphProperties m_minGlyph;
		//Rather than store both a min and a max glyph, the min glyph is stored (listed above) in this class plus an object that stores the difference between min and max
		//(listed below).  If a value in m_difference is 0 then min and max is the same.  Otherwise the max value is the min value plus the difference value.
		GlyphMappableProperties m_difference;
	};

	typedef stlplus::ntree<MinMaxGlyph> MinMaxGlyphTree;

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPH_H