#ifndef SYNGLYPHX_DATAMAPPINGFUNCTION_H
#define SYNGLYPHX_DATAMAPPINGFUNCTION_H

#include <vector>
#include <memory>
#include "inputcombinationfunction.h"

namespace SynGlyphX {

	template<typename OutputType, typename InputType = OutputType>
	class DataMappingFunction
	{
	public:
		DataMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction = nullptr);
		~DataMappingFunction();

		OutputType MapInputToOuptut(const std::vector<InputType>& input) const;

	protected:
		virtual OutputType MapCombinedInput(const InputType& input) const = 0;

		std::shared_ptr<const InputCombinationFunction<InputType>> m_inputCombinationFunction;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATAMAPPING_H
