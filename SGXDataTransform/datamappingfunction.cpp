#include "datamappingfunction.h"
#include <stdexcept>
#include <string>
#include <type_traits>
#include "color.h"

namespace SynGlyphX {

	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::DataMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction) :
		m_inputCombinationFunction(inputCombinationFunction ? inputCombinationFunction : std::make_shared<const InputCombinationFunction<InputType>>())
	{
		
	}

	template<typename OutputType, typename InputType>
	DataMappingFunction<OutputType, InputType>::~DataMappingFunction()
	{

	}

	template<typename OutputType, typename InputType>
	OutputType DataMappingFunction<OutputType, InputType>::MapInputToOuptut(const std::vector<InputType>& input) const {

		if (input.empty()) {

			throw std::invalid_argument("There must be at least one input value to map");
		}

		return MapCombinedInput(m_inputCombinationFunction->CombineInput(input));
	}

	template class DataMappingFunction < std::wstring >;
	template class DataMappingFunction < double >;
	template class DataMappingFunction < Color, double >;

} //namespace SynGlyphX