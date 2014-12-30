#ifndef SYNGLYPHX_DATAMAPPINGFUNCTION_H
#define SYNGLYPHX_DATAMAPPINGFUNCTION_H

#include <vector>
#include "inputcombinationfunction.h"

namespace SynGlyphX {

	template<typename OutputType, typename InputType = OutputType>
	class DataMappingFunction
	{
	public:
		DataMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction = new InputCombinationFunction<InputType>());
		~DataMappingFunction();

		OutputType MapInput(const std::vector<InputType>& input);

	protected:
		virtual OutputType MapCombinedInput(InputType input) = 0;

		std::shared_ptr<const InputCombinationFunction<InputType>> m_inputCombinationFunction;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPING_H
