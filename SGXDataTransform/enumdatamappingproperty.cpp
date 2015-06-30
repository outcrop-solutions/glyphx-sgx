#include "enumdatamappingproperty.h"

namespace SynGlyphX {

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>::EnumDataMappingProperty() :
		DataMappingProperty <EnumType>()
	{
	}

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>::EnumDataMappingProperty(const EnumType& initialValue) :
		DataMappingProperty<EnumType>(initialValue) {


	}

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>::EnumDataMappingProperty(const boost::property_tree::wptree& propertyTree) :
		DataMappingProperty<EnumType>(propertyTree) {


	}

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>::EnumDataMappingProperty(const EnumDataMappingProperty& prop) :
		DataMappingProperty<EnumType>(prop) {


	}

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>::~EnumDataMappingProperty()
	{
	}

	template<typename EnumType>
	EnumDataMappingProperty<EnumType>& EnumDataMappingProperty<EnumType>::operator=(const EnumDataMappingProperty& prop) {

		DataMappingProperty<EnumType>::operator=(prop);

		return *this;
	}

	template<typename EnumType>
	bool EnumDataMappingProperty<EnumType>::operator==(const EnumDataMappingProperty& prop) const {

		return DataMappingProperty<EnumType>::operator==(prop);
	}

	template<typename EnumType>
	bool EnumDataMappingProperty<EnumType>::operator!=(const EnumDataMappingProperty& prop) const {

		return !operator==(prop);
	}

	template<typename EnumType>
	bool EnumDataMappingProperty<EnumType>::IsMappingFunctionDataCompatible(MappingFunctionData::SharedPtr mappingFunctionData) const {

		MappingFunctionData::Function function = mappingFunctionData->GetFunction();
		return ((function != MappingFunctionData::Function::LinearInterpolation) && (function != MappingFunctionData::Function::LogarithmicInterpolation));
	}

	template class EnumDataMappingProperty<GlyphGeometryInfo::Shape>;
	template class EnumDataMappingProperty<VirtualTopologyInfo::Type>;

} //namespace SynGlyphX