#include "rangevaluemappingfunction.h"
#include <algorithm>

namespace SynGlyphX {
	/*
	template <typename OutputType>
	RangeMappingData<OutputType>::RangeMappingData() :
		ValueMappingData<OutputType, double>(MappingFunctionData::Function::Range2Value)
	{
	}

	template <typename OutputType>
	RangeMappingData<OutputType>::RangeMappingData(const boost::property_tree::wptree& propertyTree) :
		ValueMappingData<OutputType, double>(propertyTree) {

		for (const boost::property_tree::wptree::value_type& rangeValuePair : propertyTree.get_child(L"RangeValuePairs")) {

			Range range = rangeValuePair.second.get<Range>(L"Range");
			OutputType value = rangeValuePair.second.get<OutputType>(L"Value");
			m_mappedValues[range] = value;
		}
	}

	template <typename OutputType>
	RangeMappingData<OutputType>::RangeMappingData(const RangeMappingData& data) :
		ValueMappingData<OutputType, double>(data),
		m_mappedValues(data.m_mappedValues) {


	}

	template <typename OutputType>
	RangeMappingData<OutputType>::~RangeMappingData()
	{
	}

	template <typename OutputType>
	void RangeMappingData<OutputType>::SetMappedValue(const Range& input, const OutputType& output) {

		m_mappedValues[input] = output;
	}

	template <typename OutputType>
	void RangeMappingData<OutputType>::RemoveMappedValue(const Range& input) {

		m_mappedValues.erase(input);
	}

	template <typename OutputType>
	void RangeMappingData<OutputType>::ClearMappedValues() {

		m_mappedValues.clear();
	}

	template <typename OutputType>
	const std::map<Range, OutputType>& RangeMappingData<OutputType>::GetMappedValues() const {

		return m_mappedValues;
	}

	template <typename OutputType>
	OutputType RangeMappingData<OutputType>::GetOutputValueFromInput(const double& input) const {
		
		for (std::map<Range, OutputType>::const_iterator iT = m_mappedValues.begin(); iT != m_mappedValues.end(); ++iT) {

			if (iT->first.IsValueInRange(input)) {

				return iT->second;
			}
		}

		return m_defaultValue;
	}

	template <typename OutputType>
	boost::property_tree::wptree& RangeMappingData<OutputType>::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		using ValueMappingData < OutputType, double > ;
		boost::property_tree::wptree& mappingPropertyTree = ValueMappingData<OutputType, double>::ExportToPropertyTree(propertyTree);
		boost::property_tree::wptree& rangeValuePairsPropertyTree = mappingPropertyTree.add(L"RangeValuePairs", L"");

		for (std::map<Range, OutputType>::const_iterator keyValuePair = m_mappedValues.begin(); keyValuePair != m_mappedValues.end(); ++keyValuePair) {

			boost::property_tree::wptree& keyValuePairPropertyTree = rangeValuePairsPropertyTree.add(L"RangeValuePair", L"");
			keyValuePairPropertyTree.put<Range>(L"Range", keyValuePair->first);
			keyValuePairPropertyTree.put<OutputType>(L"Value", keyValuePair->second);
		}

		return mappingPropertyTree;
	}

	template class RangeMappingData<double>;
	template class RangeMappingData<GlyphColor>;*/

} //namespace SynGlyphX