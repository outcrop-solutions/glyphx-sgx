#ifndef SYNGLYPHX_DATAMAPPINGPROPERTY_H
#define SYNGLYPHX_DATAMAPPINGPROPERTY_H

#include "sgxdatatransform_global.h"
#include <array>
#include <boost/property_tree/ptree.hpp>
#include "glyphcolor.h"
#include "inputbinding.h"
#include <memory>
#include "linearinterpolationmappingfunction.h"

namespace SynGlyphX {

	template<typename PropertyType> //, typename MappingOutputType = PropertyType>
	class SGXDATATRANSFORM_EXPORT DataMappingProperty {

	public:
		DataMappingProperty();
		DataMappingProperty(const PropertyType& initialValue);
		DataMappingProperty(const boost::property_tree::wptree& propertyTree);
		DataMappingProperty(const DataMappingProperty& prop);
		~DataMappingProperty();

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		PropertyType& GetValue();
		const PropertyType& GetValue() const;

		InputBinding& GetBinding();
		const InputBinding& GetBinding() const;

	protected:
		PropertyType m_value;
		InputBinding m_binding;
	};

	typedef DataMappingProperty<std::pair<double, double>> NumericMappingProperty;
	typedef DataMappingProperty<std::pair<GlyphColor, GlyphColor>> ColorMappingProperty;
	typedef DataMappingProperty<std::wstring> TextMappingProperty;

	typedef std::array<NumericMappingProperty, 3> NumericMappingPropertyXYZ;

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPINGPROPERTY_H