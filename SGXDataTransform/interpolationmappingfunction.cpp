#include "interpolationmappingfunction.h"
#include "color.h"

namespace SynGlyphX {

	template<typename OutputType>
	InterpolationMappingFunction<OutputType>::InterpolationMappingFunction() :
		DataMappingFunction<OutputType, double>(),
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
	OutputType InterpolationMappingFunction<OutputType>::MapCombinedInput(double input) {

		return Interpolate(input, m_inputMin, m_inputDifference, m_outputMin, m_outputDifference);
	}

	template<>
	Color InterpolationMappingFunction<Color>::MapCombinedInput(double input) {

		Color minHSV = Color::ConvertRGBtoHSV(m_outputMin);
		Color differenceHSV = Color::ConvertRGBtoHSV(m_outputDifference);

		Color outputHSV;
		outputHSV.Set(0, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));
		outputHSV.Set(1, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));
		outputHSV.Set(2, static_cast<short>(Interpolate(input, m_inputMin, m_inputDifference, minHSV[0], differenceHSV[0])));

		return Color::ConvertHSVtoRGB(outputHSV);
	}

} //namespace SynGlyphX