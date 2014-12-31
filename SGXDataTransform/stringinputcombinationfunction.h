#ifndef SYNGLYPHX_STRINGINPUTCOMBINATIONFUNCTION_H
#define SYNGLYPHX_STRINGINPUTCOMBINATIONFUNCTION_H

#include "inputcombinationfunction.h"
#include <string>

namespace SynGlyphX {

	class StringInputCombinationFunction : public InputCombinationFunction<std::wstring>
	{
	public:
		StringInputCombinationFunction();
		virtual ~StringInputCombinationFunction();

		virtual std::wstring CombineInput(const std::vector<std::wstring>& inputs) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_STRINGINPUTCOMBINATIONFUNCTION_H