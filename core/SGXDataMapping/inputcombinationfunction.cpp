#include "inputcombinationfunction.h"
#include <string>

namespace SynGlyphX {

	template<typename InputType>
	InputCombinationFunction<InputType>::InputCombinationFunction()
	{
	}

	template<typename InputType>
	InputCombinationFunction<InputType>::~InputCombinationFunction()
	{
	}

	template<typename InputType>
	InputType InputCombinationFunction<InputType>::CombineInput(const std::vector<InputType>& inputs) const {

		return inputs[0];
	}

	template class InputCombinationFunction < double >;
	template class InputCombinationFunction < std::wstring >;

} //namespace SynGlyphX
