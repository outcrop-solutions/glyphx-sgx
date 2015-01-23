#include "logarithmicinterpolationmappingfunction.h"

namespace SynGlyphX {
	/*
	LogarithmicInterpolationMappingFunction::LogarithmicInterpolationMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction) :
		InterpolationMappingFunction<double>(inputCombinationFunction ? inputCombinationFunction : std::make_shared<const InputCombinationFunction<double>>())
	{
	}

	LogarithmicInterpolationMappingFunction::~LogarithmicInterpolationMappingFunction()
	{
	}

	double LogarithmicInterpolationMappingFunction::Interpolate(double input, double inputMin, double inputDifference, double outputMin, double outputDifference) const {

		if (std::abs(inputDifference) > 0.001) {

			return (outputMin + ((std::log10(input - inputMin + 1) / std::log10(inputDifference + 1)) * outputDifference));
		}
		else {

			return outputMin;
		}
	}
	*/
} //namespace SynGlyphX