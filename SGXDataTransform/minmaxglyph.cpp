#include "minmaxglyph.h"

namespace SynGlyphX {

	MinMaxGlyph::MinMaxGlyph(const GlyphProperties& minGlyph) :
		m_minGlyph(minGlyph),
		m_difference(GlyphMappableProperties::GetPropertiesZero())
	{
	}

	MinMaxGlyph::MinMaxGlyph(const boost::property_tree::wptree& propertyTree) :
		m_difference(GlyphMappableProperties::GetPropertiesZero()) {


	}

	MinMaxGlyph::MinMaxGlyph(const MinMaxGlyph& glyph) :
		m_minGlyph(glyph.m_minGlyph),
		m_difference(glyph.m_difference) {

	}

	MinMaxGlyph::~MinMaxGlyph()
	{
	}

	MinMaxGlyph& MinMaxGlyph::operator=(const MinMaxGlyph& glyph) {

		m_minGlyph = glyph.m_minGlyph;
		m_difference = glyph.m_difference;

		return *this;
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

	void MinMaxGlyph::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {

	}

} //namespace SynGlyphX