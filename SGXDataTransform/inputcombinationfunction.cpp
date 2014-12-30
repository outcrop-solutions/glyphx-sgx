#include "inputcombinationfunction.h"

namespace SynGlyphX {

	template<typename InputType>
	InputCombinationFunction::InputCombinationFunction()
	{
	}

	template<typename InputType>
	InputCombinationFunction::~InputCombinationFunction()
	{
	}

	template<typename InputType>
	InputType InputCombinationFunction::CombineInput(const std::vector<InputType>& inputs) {

		return inputs[0];
	}

} //namespace SynGlyphX