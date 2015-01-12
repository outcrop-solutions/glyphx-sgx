#ifndef SYNGLYPHX_DATAMAPPINGPROPERTY_H
#define SYNGLYPHX_DATAMAPPINGPROPERTY_H

#include "sgxdatatransform_global.h"
#include <array>
#include <boost/property_tree/ptree.hpp>
#include "color.h"
#include "inputbinding.h"

namespace SynGlyphX {

	template<typename PropertyType>
	class SGXDATATRANSFORM_EXPORT DataMappingProperty {

	public:
		PropertyType value;
		InputBinding binding;

		DataMappingProperty();
		DataMappingProperty(const PropertyType& initialValue);
		DataMappingProperty(const boost::property_tree::wptree& propertyTree);
		DataMappingProperty(const DataMappingProperty& prop);
		~DataMappingProperty();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
	};

	typedef DataMappingProperty<std::pair<double, double>> NumericMappingProperty;
	typedef DataMappingProperty<std::pair<Color, Color>> ColorMappingProperty;
	typedef DataMappingProperty<std::wstring> TextMappingProperty;
	typedef std::array<NumericMappingProperty, 3> NumericMappingPropertyXYZ;

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGPROPERTY_H