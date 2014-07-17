#include "minmaxglyph.h"

namespace SynGlyphX {

	MinMaxGlyph::MinMaxGlyph(const GlyphProperties& minGlyph) :
		m_minGlyph(minGlyph),
		m_difference(GlyphMappableProperties::GetPropertiesZero())
	{
	}


	MinMaxGlyph::~MinMaxGlyph()
	{
	}

	const GlyphProperties& MinMaxGlyph::GetMinGlyph() const {

		return m_minGlyph;
	}

	const GlyphMappableProperties& MinMaxGlyph::GetDifference() const {

		return m_difference;
	}

	void MinMaxGlyph::SetMinGlyphProperties(const GlyphMappableProperties& properties) {

		m_minGlyph.GlyphMappableProperties::operator=(properties);
	}

	void MinMaxGlyph::SetDifference(const GlyphMappableProperties& difference) {

		m_difference = difference;
	}

} //namespace SynGlyphX