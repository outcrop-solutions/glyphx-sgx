#ifndef SYNGLYPHX_MINMAXGLYPH_H
#define SYNGLYPHX_MINMAXGLYPH_H

#include "glyphproperties.h"
#include "glyph.h"
#include <vector>
#include <containers/ntree.hpp>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	class MinMaxGlyph
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;

		MinMaxGlyph(const GlyphProperties& minGlyph);
		MinMaxGlyph(const boost::property_tree::wptree& propertyTree);
		MinMaxGlyph(const MinMaxGlyph& glyph);
		~MinMaxGlyph();

		MinMaxGlyph& operator=(const MinMaxGlyph& glyph);

		const GlyphProperties& GetMinGlyph() const;
		const GlyphMappableProperties& GetDifference() const;

		void SetMinGlyphProperties(const GlyphMappableProperties& glyph);
		void SetDifference(const GlyphMappableProperties& difference);

		PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

	private:
		void AddVector3ToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, const Vector3& min, const Vector3& difference) const;
		void AddValueToPropertyTree(boost::property_tree::wptree& propertyTreeParent, const std::wstring& name, double min, double difference) const;

		GlyphProperties m_minGlyph;
		//Rather than store both a min and a max glyph, the min glyph is stored (listed above) in this class plus an object that stores the difference between min and max
		//(listed below).  If a value in m_difference is 0 then min and max is the same.  Otherwise the max value is the min value plus the difference value.
		GlyphMappableProperties m_difference;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_MINMAXGLYPH_H