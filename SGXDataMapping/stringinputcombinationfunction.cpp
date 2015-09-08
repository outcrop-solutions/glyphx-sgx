#include "stringinputcombinationfunction.h"

namespace SynGlyphX {

	StringInputCombinationFunction::StringInputCombinationFunction() :
		InputCombinationFunction<std::wstring>()
	{
	}


	StringInputCombinationFunction::~StringInputCombinationFunction()
	{
	}

	std::wstring StringInputCombinationFunction::CombineInput(const std::vector<std::wstring>& inputs) const {

		std::wstring combinedInput;
		for (const std::wstring& input : inputs) {

			combinedInput += input + L" ";
		}

		return combinedInput;
	}

} //namespace SynGlyphX