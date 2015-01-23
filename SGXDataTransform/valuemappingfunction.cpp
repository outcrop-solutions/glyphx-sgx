#include "valuemappingfunction.h"
#include "glyphcolor.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType>
	ValueMappingData<OutputType, InputType>::ValueMappingData(Function function) :
		MappingFunctionData(function),
		m_defaultValue(OutputType())
	{
	}

	template <typename OutputType, typename InputType>
	ValueMappingData<OutputType, InputType>::ValueMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree),
		m_defaultValue(propertyTree.get<OutputType>(L"Default")) {

	}

	template <>
	ValueMappingData<GlyphColor, double>::ValueMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree),
		m_defaultValue(propertyTree.get_child(L"Default")) {

	}

	template <>
	ValueMappingData<GlyphColor, std::wstring>::ValueMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree),
		m_defaultValue(propertyTree.get_child(L"Default")) {

	}

	template <typename OutputType, typename InputType>
	ValueMappingData<OutputType, InputType>::ValueMappingData(const ValueMappingData& data) :
		MappingFunctionData(data),
		m_defaultValue(data.m_defaultValue) {

	}

	template <typename OutputType, typename InputType>
	ValueMappingData<OutputType, InputType>::~ValueMappingData()
	{
	}

	template <typename OutputType, typename InputType>
	ValueMappingData<OutputType, InputType>& ValueMappingData<OutputType, InputType>::operator=(const ValueMappingData& data) {

		MappingFunctionData::operator=(data);
		m_defaultValue = data.m_defaultValue;

		return *this;
	}

	template <typename OutputType, typename InputType>
	void ValueMappingData<OutputType, InputType>::SetDefaultValue(const OutputType& defaultValue) {

		m_defaultValue = defaultValue;
	}

	template <typename OutputType, typename InputType>
	const OutputType& ValueMappingData<OutputType, InputType>::GetDefaultValue() const {

		return m_defaultValue;
	}

	template <>
	MappingFunctionData::Input ValueMappingData<double, double>::GetSupportedInput() const {

		return MappingFunctionData::Input::Numeric;
	}

	template <>
	MappingFunctionData::Input ValueMappingData<GlyphColor, double>::GetSupportedInput() const {

		return MappingFunctionData::Input::Numeric;
	}

	template <typename OutputType, typename InputType>
	MappingFunctionData::Input ValueMappingData<OutputType, InputType>::GetSupportedInput() const {

		return MappingFunctionData::Input::Text;
	}

	template <typename OutputType, typename InputType>
	boost::property_tree::wptree& ValueMappingData<OutputType, InputType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {

		boost::property_tree::wptree& functionDataPropertyTree = MappingFunctionData::ExportToPropertyTree(propertyTree);
		functionDataPropertyTree.put(L"Default", m_defaultValue);
		return functionDataPropertyTree;
	}

	template <>
	boost::property_tree::wptree& ValueMappingData<GlyphColor, double>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {

		boost::property_tree::wptree& functionDataPropertyTree = MappingFunctionData::ExportToPropertyTree(propertyTree);
		m_defaultValue.ExportToPropertyTree(functionDataPropertyTree.add(L"Default", L""));
		return functionDataPropertyTree;
	}

	template <>
	boost::property_tree::wptree& ValueMappingData<GlyphColor, std::wstring>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {

		boost::property_tree::wptree& functionDataPropertyTree = MappingFunctionData::ExportToPropertyTree(propertyTree);
		m_defaultValue.ExportToPropertyTree(functionDataPropertyTree.add(L"Default", L""));
		return functionDataPropertyTree;
	}

	template class ValueMappingData < double, double > ;
	template class ValueMappingData < double, std::wstring > ;
	template class ValueMappingData < GlyphColor, double >;
	template class ValueMappingData < GlyphColor, std::wstring >;

} //namespace SynGlyphX