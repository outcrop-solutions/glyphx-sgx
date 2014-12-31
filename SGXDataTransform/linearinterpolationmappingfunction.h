#ifndef SYNGLYPHX_LINEARINTERPOLATIONMAPPINGFUNCTION_H
#define SYNGLYPHX_LINEARINTERPOLATIONMAPPINGFUNCTION_H

#include "interpolationmappingfunction.h"

namespace SynGlyphX {

	class LinearInterpolationMappingFunction : public InterpolationMappingFunction<double>
	{
	public:
		LinearInterpolationMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction = nullptr);
		virtual ~LinearInterpolationMappingFunction();

	protected:
		virtual double Interpolate(double input, double inputMin, double inputDifference, double outputMin, double outputDifference) const;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_LINEARINTERPOLATIONMAPPINGFUNCTION_H