#ifndef SYNGLYPHX_SINGLEVALUEMAPPINGFUNCTION_H
#define SYNGLYPHX_SINGLEVALUEMAPPINGFUNCTION_H

#include "valuemappingfunction.h"
#include <unordered_map>

namespace SynGlyphX {

	template <typename OutputType, typename InputType = OutputType>
	class SingleValueMappingFunction : public ValueMappingFunction<OutputType, InputType>
	{
	public:
		SingleValueMappingFunction(std::shared_ptr<const InputCombinationFunction<InputType>> inputCombinationFunction = nullptr);
		virtual ~SingleValueMappingFunction();

		void SetMappedValue(const InputType& input, const OutputType& output);
		void RemoveMappedValue(const InputType& input);
		void ClearMappedValues();
		const std::unordered_map<InputType, OutputType>& GetMappedValues() const;

	protected:
		virtual OutputType MapCombinedInput(const InputType& input) const;

		std::unordered_map<InputType, OutputType> m_mappedValues;
	};

} // namespace SynGlyphX

#endif //SYNGLYPHX_SINGLEVALUEMAPPINGFUNCTION_H