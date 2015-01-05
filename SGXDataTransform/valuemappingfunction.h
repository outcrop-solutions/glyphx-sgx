#ifndef SYNGLYPHX_VALUEMAPPINGFUNCTION_H
#define SYNGLYPHX_VALUEMAPPINGFUNCTION_H

#include "datamappingfunction.h"

namespace SynGlyphX {

	template <typename OutputType, typename InputType = OutputType>
	class ValueMappingFunction : public DataMappingFunction<OutputType, InputType>
	{
	public:
		ValueMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction = nullptr);
		virtual ~ValueMappingFunction();

		void SetDefaultValue(const OutputType& defaultValue);
		const OutputType& GetDefaultValue() const;

	protected:
		OutputType m_defaultValue;
	};

} // namespace SynGlyphX

#endif //SYNGLYPHX_VALUEMAPPINGFUNCTION_H