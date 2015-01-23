#include "linearinterpolationmappingfunction.h"

namespace SynGlyphX {
	/*
	LinearInterpolationMappingFunction::LinearInterpolationMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction) :
		InterpolationMappingFunction<double>(inputCombinationFunction ? inputCombinationFunction : std::make_shared<const InputCombinationFunction<double>>())
	{
	}

	LinearInterpolationMappingFunction::~LinearInterpolationMappingFunction()
	{
	}

	double LinearInterpolationMappingFunction::Interpolate(double input, double inputMin, double inputDifference, double outputMin, double outputDifference) const {

		if (std::abs(inputDifference) > 0.001) {

			return (outputMin + (((input - inputMin) / inputDifference) * outputDifference));
		}
		else {

			return outputMin;
		}
	}
	*/
} //namespace SynGlyphX