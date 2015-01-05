#include "valuemappingfunction.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType>
	ValueMappingFunction<OutputType, InputType>::ValueMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction) :
		DataMappingFunction<OutputType, InputType>(inputCombinationFunction),
		m_defaultValue(OutputValue())
	{
	}

	template <typename OutputType, typename InputType>
	ValueMappingFunction<OutputType, InputType>::~ValueMappingFunction()
	{
	}

	template <typename OutputType, typename InputType>
	void ValueMappingFunction<OutputType, InputType>::SetDefaultValue(const OutputType& defaultValue) {

		m_defaultValue = defaultValue;
	}

	template <typename OutputType, typename InputType>
	const OutputType& ValueMappingFunction<OutputType, InputType>::GetDefaultValue() const {

		return m_defaultValue;
	}

} //namespace SynGlyphX