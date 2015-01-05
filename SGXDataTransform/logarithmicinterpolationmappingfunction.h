#ifndef SYNGLYPHX_LOGARITHMICINTERPOLATIONMAPPINGFUNCTION_H
#define SYNGLYPHX_LOGARITHMICINTERPOLATIONMAPPINGFUNCTION_H

#include "interpolationmappingfunction.h"

namespace SynGlyphX {

	class LogarithmicInterpolationMappingFunction : public InterpolationMappingFunction<double>
	{
	public:
		LogarithmicInterpolationMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction = nullptr);
		virtual ~LogarithmicInterpolationMappingFunction();

	protected:
		virtual double Interpolate(double input, double inputMin, double inputDifference, double outputMin, double outputDifference) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_LOGARITHMICINTERPOLATIONMAPPINGFUNCTION_H