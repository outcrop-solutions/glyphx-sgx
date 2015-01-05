#include "singlevaluemappingfunction.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType>
	SingleValueMappingFunction<OutputType, InputType>::SingleValueMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction) :
		ValueMappingFunction<OutputType, InputType>(inputCombinationFunction)
	{
	}

	template <typename OutputType, typename InputType>
	SingleValueMappingFunction<OutputType, InputType>::~SingleValueMappingFunction()
	{
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingFunction<OutputType, InputType>::SetMappedValue(const InputType& input, const OutputType& output) {

		m_mappedValues[input] = output;
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingFunction<OutputType, InputType>::RemoveMappedValue(const InputType& input) {

		m_mappedValues.erase(input);
	}

	template <typename OutputType, typename InputType>
	void SingleValueMappingFunction<OutputType, InputType>::ClearMappedValues() {

		m_mappedValues.clear();
	}

	template <typename OutputType, typename InputType>
	const std::unordered_map<InputType, OutputType>& SingleValueMappingFunction<OutputType, InputType>::GetMappedValues() const {

		return m_mappedValues;
	}

	template <typename OutputType, typename InputType>
	OutputType SingleValueMappingFunction<OutputType, InputType>::MapCombinedInput(const InputType& input) const {

		std::unordered_map<InputType, OutputType>::const_iterator output = m_mappedValues.find(input);
		if (output == m_mappedValues.end()) {

			return m_defaultValue;
		}
		else {

			return output->second;
		}
	}

} //namespace SynGlyphX