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
		enum MappableField {
			PositionX = 0,
			PositionY,
			PositionZ,
			RotationX,
			RotationY,
			RotationZ,
			ScaleX,
			ScaleY,
			ScaleZ,
			Color,
			Transparency,
			Tag,
			Description,
			RotationRateX,
			RotationRateY,
			RotationRateZ,
			MappableFieldSize //This needs to be at the bottom to get the number of enums
		};

		DataMappingGlyph();
		DataMappingGlyph(const DataMappingGlyph& glyph);
		DataMappingGlyph(const Glyph& glyph);
		DataMappingGlyph(const boost::property_tree::wptree& propertyTree);
		~DataMappingGlyph();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		SynGlyphX::Glyph GetMinGlyph() const;
		SynGlyphX::Glyph GetDifference() const;
		SynGlyphX::Glyph GetMaxGlyph() const;

		DataMappingGlyph& operator=(const DataMappingGlyph& glyph);
		bool operator==(const DataMappingGlyph& glyph) const;
		bool operator!=(const DataMappingGlyph& glyph) const;

		void SetInputBinding(MappableField field, const InputBinding& binding);
		void ClearInputBinding(MappableField field);

		bool IsPositionXYBoundToInputFields() const;

	protected:
		void GetXYZNumericPropertiesFromPropertyTree(const boost::property_tree::wptree& propertyTree, NumericMappingPropertyXYZ& prop);
		void AddXYZNumericPropertiesToPropertyTree(boost::property_tree::wptree& propertyTree, const NumericMappingPropertyXYZ& prop) const;
		InputBinding& SelectInputBinding(MappableField field);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGGLYPH_H