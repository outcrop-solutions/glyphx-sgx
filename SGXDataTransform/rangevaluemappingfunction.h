#ifndef SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H
#define SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H

#include "valuemappingfunction.h"
#include "range.h"
#include <map>

namespace SynGlyphX {

	template <typename OutputType>
	class RangeValueMappingFunction : public ValueMappingFunction<OutputType, Range>
	{
	public:
		RangeValueMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction = nullptr);
		virtual ~RangeValueMappingFunction();

		void SetMappedValue(const Range& input, const OutputType& output);
		void RemoveMappedValue(const Range& input);
		void ClearMappedValues();
		const std::map<Range, OutputType>& GetMappedValues() const;

	protected:
		virtual OutputType MapCombinedInput(const double& input) const;

		std::map<Range, OutputType> m_mappedValues;
	};

} // namespace SynGlyphX

#endif //SYNGLYPHX_RANGEVALUEMAPPINGFUNCTION_H