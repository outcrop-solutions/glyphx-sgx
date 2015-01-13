#ifndef SYNGLYPHX_DATAMAPPINGGLYPH_H
#define SYNGLYPHX_DATAMAPPINGGLYPH_H

#include "sgxdatatransform_global.h"
#include "glyphgraph.h"
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "datamappingproperty.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataMappingGlyph : public GlyphTemplate<NumericMappingProperty, ColorMappingProperty, TextMappingProperty> {

	public:

		DataMappingGlyph();
		DataMappingGlyph(const DataMappingGlyph& glyph);
		DataMappingGlyph(const Glyph& glyph);
		DataMappingGlyph(const boost::property_tree::wptree& propertyTree);
		~DataMappingGlyph();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		SynGlyphX::Glyph GetMinGlyph() const;
		SynGlyphX::Glyph GetDifference() const;
		SynGlyphX::Glyph GetMaxGlyph() const;

		/*MinMaxGlyph& operator=(const MinMaxGlyph& glyph);
		bool operator==(const MinMaxGlyph& glyph) const;
		bool operator!=(const MinMaxGlyph& glyph) const;

		void SetMinGlyph(const SynGlyphXANTz::GlyphProperties& glyph);
		void SetDifference(const SynGlyphXANTz::GlyphNumericMappableProperties& difference);

		const InputBinding& GetInputBinding(unsigned int index) const;
		void SetInputBinding(unsigned int index, const InputBinding& binding);
		void ClearInputBinding(unsigned int index);*/

		bool IsPositionXYBoundToInputFields() const;

	private:
		void GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericMappingPropertyXYZ& prop);
		void AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericMappingPropertyXYZ& prop) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPH_H