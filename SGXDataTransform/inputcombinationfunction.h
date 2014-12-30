#ifndef SYNGLYPHX_INPUTCOMBINATIONFUNCTION_H
#define SYNGLYPHX_INPUTCOMBINATIONFUNCTION_H

namespace SynGlyphX {

	template<typename InputType>
	class InputCombinationFunction
	{
	public:
		InputCombinationFunction();
		virtual ~InputCombinationFunction();

		virtual InputType CombineInput(const std::vector<InputType>& inputs);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_INPUTCOMBINATIONFUNCTION_H