#include "rangevaluemappingfunction.h"
#include <algorithm>

namespace SynGlyphX {

	template <typename OutputType>
	RangeValueMappingFunction<OutputType>::RangeValueMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction) :
		ValueMappingFunction<OutputType, Range>(inputCombinationFunction)
	{
	}

	template <typename OutputType>
	RangeValueMappingFunction<OutputType>::~RangeValueMappingFunction()
	{
	}

	template <typename OutputType>
	void RangeValueMappingFunction<OutputType>::SetMappedValue(const Range& input, const OutputType& output) {

		m_mappedValues[input] = output;
	}

	template <typename OutputType>
	void RangeValueMappingFunction<OutputType>::RemoveMappedValue(const Range& input) {

		m_mappedValues.erase(input);
	}

	template <typename OutputType>
	void RangeValueMappingFunction<OutputType>::ClearMappedValues() {

		m_mappedValues.clear();
	}

	template <typename OutputType>
	const std::map<Range, OutputType>& RangeValueMappingFunction<OutputType>::GetMappedValues() const {

		return m_mappedValues;
	}

	template <typename OutputType>
	OutputType RangeValueMappingFunction<OutputType>::MapCombinedInput(const double& input) const {

		std::map<Range, OutputType>::const_iterator output = std::lower_bound(m_mappedValues.begin(), m_mappedValues.end(), input);
		if (output == m_mappedValues.end()) {

			return m_defaultValue;
		}
		else {

			return output->second;
		}
	}

} //namespace SynGlyphX