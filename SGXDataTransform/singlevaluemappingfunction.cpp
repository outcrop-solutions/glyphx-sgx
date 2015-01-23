#include "singlevaluemappingfunction.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType>
	SingleValueMappingData<OutputType, InputType>::SingleValueMappingData() :
		ValueMappingData<OutputType, InputType>(MappingFunctionData::Function::Text2Value)
	{
	}

	template <>
	SingleValueMappingData<double, double>::SingleValueMappingData() :
		ValueMappingData<double, double>(MappingFunctionData::Function::Numeric2Value)
	{
	}

	template <>
	SingleValueMappingData<GlyphColor, double>::SingleValueMappingData() :
		ValueMappingData<GlyphColor, double>(MappingFunctionData::Function::Numeric2Value)
	{
	}

	template <typename OutputType, typename InputType>
	SingleValueMappingData<OutputType, InputType>::SingleValueMappingData(const boost::property_tree::wptree& propertyTree) :
		ValueMappingData<OutputType, InputType>(propertyTree) {

		for (const boost::property_tree::wptree::value_type& keyValuePair : propertyTree.get_child(L"KeyValuePairs")) {

			InputType key = keyValuePair.second.get<InputType>(L"key");
			OutputType value = keyValuePair.second.get<OutputType>(L"value");
			m_mappedValues[key] = value;
		}
	}

	template <typename OutputType, typename InputType>
	SingleValueMappingData<OutputType, InputType>::SingleValueMappingData(const SingleValueMappingData& data) :
		ValueMappingData(data),
		m_mappedValues(data.m_mappedValues) {

	}

	template <typename OutputType, typename InputType>
	SingleValueMappingData<OutputType, InputType>::~SingleValueMappingData()
	{
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingData<OutputType, InputType>::SetMappedValue(const InputType& input, const OutputType& output) {

		m_mappedValues[input] = output;
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingData<OutputType, InputType>::RemoveMappedValue(const InputType& input) {

		m_mappedValues.erase(input);
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingData<OutputType, InputType>::ClearMappedValues() {

		m_mappedValues.clear();
	}

	template <typename OutputType, typename InputType>
	const std::unordered_map<InputType, OutputType>& SingleValueMappingData<OutputType, InputType>::GetMappedValues() const {

		return m_mappedValues;
	}

	template <typename OutputType, typename InputType>
	OutputType SingleValueMappingData<OutputType, InputType>::GetOutputValueFromInput(const InputType& input) const {

		std::unordered_map<InputType, OutputType>::const_iterator output = m_mappedValues.find(input);
		if (output == m_mappedValues.end()) {

			return m_defaultValue;
		}
		else {

			return output->second;
		}
	}

	template <typename OutputType, typename InputType>
	bool SingleValueMappingData<OutputType, InputType>::IsInputInMappedValues(const InputType& input) const {

		return (m_mappedValues.count(input) > 0);
	}

	template class SingleValueMappingData<double, double>;
	template class SingleValueMappingData<double, std::wstring>;
	template class SingleValueMappingData<GlyphColor, double>;
	template class SingleValueMappingData<GlyphColor, std::wstring>;

} //namespace SynGlyphX