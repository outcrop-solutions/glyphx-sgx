#include "interpolationmappingfunction.h"
#include "glyphcolor.h"

namespace SynGlyphX {

	template<typename OutputType>
	InterpolationMappingFunction<OutputType>::InterpolationMappingFunction(std::shared_ptr<const InputCombinationFunction<double>> inputCombinationFunction) :
		DataMappingFunction<OutputType, double>(inputCombinationFunction ? inputCombinationFunction : std::make_shared<const InputCombinationFunction<double>>()),
		m_outputMin(OutputType()),
		m_outputDifference(OutputType()),
		m_inputMin(0.0),
		m_inputDifference(0.0)
	{
	}

	template<typename OutputType>
	InterpolationMappingFunction<OutputType>::~InterpolationMappingFunction()
	{
	}

	template<typename OutputType>
	void InterpolationMappingFunction<OutputType>::SetOutputMinAndDifference(OutputType min, OutputType difference) {

		m_outputMin = min;
		m_outputDifference = difference;
	}

	template<typename OutputType>
	void InterpolationMappingFunction<OutputType>::SetInputMinAndDifference(double min, double difference) {

		m_inputMin = min;
		m_inputDifference = difference;
	}

	template<typename OutputType>
	OutputType InterpolationMappingFunction<OutputType>::MapCombinedInput(const double& input) const {

		return Interpolate(input, m_inputMin, m_inputDifference, m_outputMin, m_outputDifference);
	}

	template<>
	GlyphColor InterpolationMappingFunction<GlyphColor>::MapCombinedInput(const double& input) const {

		GlyphColor minHSV = GlyphColor::ConvertRGBtoHSV(m_outputMin);
		GlyphColor differenceHSV = GlyphColor::ConvertRGBtoHSV(m_outputDifference);

		GlyphColor outputHSV;
		outputHSV.Set(0, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));
		outputHSV.Set(1, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));
		outputHSV.Set(2, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));

		return GlyphColor::ConvertHSVtoRGB(outputHSV);
	}

	template class InterpolationMappingFunction < double >;
	template class InterpolationMappingFunction < GlyphColor >;

} //namespace SynGlyphX