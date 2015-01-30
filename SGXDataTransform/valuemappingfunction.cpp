#include "valuemappingfunction.h"
#include "glyphcolor.h"
#include <type_traits>

namespace SynGlyphX {

	template <typename OutputType, typename InputType, typename KeyType>
	ValueMappingData<OutputType, InputType, KeyType>::ValueMappingData() :
		MappingFunctionData(std::is_same<std::wstring, InputType>::value ? MappingFunctionData::Function::Text2Value : 
			(std::is_same<Range, KeyType>::value ? MappingFunctionData::Function::Range2Value : MappingFunctionData::Function::Numeric2Value)),
		m_defaultValue(OutputType())
	{
	}

	template <typename OutputType, typename InputType, typename KeyType>
	ValueMappingData<OutputType, InputType, KeyType>::ValueMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree),
		m_defaultValue(propertyTree.get<OutputType>(L"Default")) {

		for (const boost::property_tree::wptree::value_type& keyValuePair : propertyTree.get_child(L"KeyValuePairs")) {

			KeyType key = keyValuePair.second.get<KeyType>(L"Key");
			OutputType value = keyValuePair.second.get<OutputType>(L"Value");
			m_mappedValues[key] = value;
		}

	}

	template <typename OutputType, typename InputType, typename KeyType>
	ValueMappingData<OutputType, InputType, KeyType>::ValueMappingData(const ValueMappingData& data) :
		MappingFunctionData(data),
		m_defaultValue(data.m_defaultValue),
		m_mappedValues(data.m_mappedValues) {

	}

	template <typename OutputType, typename InputType, typename KeyType>
	ValueMappingData<OutputType, InputType, KeyType>::~ValueMappingData()
	{
	}

	template <typename OutputType, typename InputType, typename KeyType>
	ValueMappingData<OutputType, InputType, KeyType>& ValueMappingData<OutputType, InputType, KeyType>::operator=(const ValueMappingData& data) {

		MappingFunctionData::operator=(data);
		m_defaultValue = data.m_defaultValue;
		m_mappedValues = data.m_mappedValues;

		return *this;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	void ValueMappingData<OutputType, InputType, KeyType>::SetDefaultValue(const OutputType& defaultValue) {

		m_defaultValue = defaultValue;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	const OutputType& ValueMappingData<OutputType, InputType, KeyType>::GetDefaultValue() const {

		return m_defaultValue;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	MappingFunctionData::Input ValueMappingData<OutputType, InputType, KeyType>::GetSupportedInput() const {

		if (std::is_arithmetic<InputType>::value) {

			return MappingFunctionData::Input::Numeric;
		}
		else {

			return MappingFunctionData::Input::Text;
		}
	}

	template <typename OutputType, typename InputType, typename KeyType>
	boost::property_tree::wptree& ValueMappingData<OutputType, InputType, KeyType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& functionDataPropertyTree = this->MappingFunctionData::ExportToPropertyTree(propertyTree);
		functionDataPropertyTree.put(L"Default", m_defaultValue);

		boost::property_tree::wptree& keyValuePairsPropertyTree = functionDataPropertyTree.add(L"KeyValuePairs", L"");

		for (std::map<KeyType, OutputType>::const_iterator keyValuePair = m_mappedValues.begin(); keyValuePair != m_mappedValues.end(); ++keyValuePair) {

			boost::property_tree::wptree& keyValuePairPropertyTree = keyValuePairsPropertyTree.add(L"KeyValuePair", L"");
			keyValuePairPropertyTree.put<KeyType>(L"Key", keyValuePair->first);
			keyValuePairPropertyTree.put<OutputType>(L"Value", keyValuePair->second);
		}

		return functionDataPropertyTree;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	OutputType ValueMappingData<OutputType, InputType, KeyType>::GetOutputValueFromInput(const InputType& input) const {

		for (std::map<KeyType, OutputType>::const_iterator iT = m_mappedValues.begin(); iT != m_mappedValues.end(); ++iT) {

			if (DoesInputMatch(iT->first, input)) {

				return iT->second;
			}
		}

		return m_defaultValue;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	bool ValueMappingData<OutputType, InputType, KeyType>::NeedsDataOtherThanMinMax() const {

		return true;
	}

	template <typename OutputType, typename InputType, typename KeyType>
	MappingFunctionData::Output ValueMappingData<OutputType, InputType, KeyType>::GetSupportedOutput() const {

		if (std::is_same<GlyphColor, OutputType>::value) {

			return Output::Color;
		}
		else {

			return Output::Numeric;
		}
	}

	template <typename OutputType, typename InputType, typename KeyType>
	bool ValueMappingData<OutputType, InputType, KeyType>::DoesInputMatch(const Range& range, const double& input) const {

		return (range.IsValueInRange(input));
	}

	template <typename OutputType, typename InputType, typename KeyType>
	bool ValueMappingData<OutputType, InputType, KeyType>::DoesInputMatch(const std::wstring& key, const std::wstring& input) const {

		return (key == input);
	}

	template <typename OutputType, typename InputType, typename KeyType>
	bool ValueMappingData<OutputType, InputType, KeyType>::DoesInputMatch(const double& key, const double& input) const {

		return (std::abs(input - key) < 0.001);
	}

	template class ValueMappingData < double, double > ;
	template class ValueMappingData < double, std::wstring > ;
	template class ValueMappingData < GlyphColor, double >;
	template class ValueMappingData < GlyphColor, std::wstring >;
	template class ValueMappingData < double, double, Range >;
	template class ValueMappingData < GlyphColor, double, Range >;

} //namespace SynGlyphX