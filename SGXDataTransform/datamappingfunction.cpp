#include "datamappingfunction.h"
#include <stdexcept>
#include <string>
#include <type_traits>

namespace SynGlyphX {

	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::DataMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction) :
		m_inputCombinationFunction(nullptr)
	{
		if (!strategy) {

			throw std::invalid_argument("Strategy parameter must not be null");
		}

		m_inputCombinationFunction = inputCombinationFunction;
	}

	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::~DataMappingFunction()
	{

	}

	template<typename OutputType, typename InputType>
	DataMappingType DataMappingFunction<OutputType, InputType>::MapData(const std::vector<InputType>& input) {

		if (input.empty()) {

			throw std::invalid_argument("There must be at least one input value to map");
		}

		return MapCombinedInput(m_inputCombinationFunction->CombineInput(input));
	}

} //namespace SynGlyphX