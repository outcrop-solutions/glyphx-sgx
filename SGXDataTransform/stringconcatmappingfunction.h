#ifndef SYNGLYPHX_STRINGCONCATMAPPINGFUNCTION_H
#define SYNGLYPHX_STRINGCONCATMAPPINGFUNCTION_H

#include "datamappingfunction.h"
#include "stringinputcombinationfunction.h"

namespace SynGlyphX {

	class StringConcatMappingFunction : public DataMappingFunction<std::wstring>
	{
	public:
		StringConcatMappingFunction(std::shared_ptr<const StringInputCombinationFunction> inputCombinationFunction = nullptr);
		virtual ~StringConcatMappingFunction();

	protected:
		virtual std::wstring MapCombinedInput(const std::wstring& input);
	};

} //namepsace SynGlyphX

#endif //SYNGLYPHX_STRINGCONCATMAPPINGFUNCTION_H