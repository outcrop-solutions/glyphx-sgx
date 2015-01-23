#include "rangevaluemappingfunction.h"
#include <algorithm>

namespace SynGlyphX {

	template <typename OutputType>
	RangeMappingData<OutputType>::RangeMappingData() :
		ValueMappingData<OutputType, double>(MappingFunctionData::Function::Range2Value)
	{
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

	template class RangeMappingData<double>;
	template class RangeMappingData<GlyphColor>;

} //namespace SynGlyphX