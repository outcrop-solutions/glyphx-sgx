#include "interpolationmappingfunction.h"
#include "glyphcolor.h"

namespace SynGlyphX {
	
	InterpolationMappingData::InterpolationMappingData(bool useLogarithmic) :
		MappingFunctionData(useLogarithmic ? MappingFunctionData::Function::LogarithmicInterpolation : MappingFunctionData::Function::LinearInterpolation) {


	}

	InterpolationMappingData::InterpolationMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree) {


	}

	InterpolationMappingData::InterpolationMappingData(const InterpolationMappingData& data) :
		MappingFunctionData(data) {


	}
	
	InterpolationMappingData::~InterpolationMappingData() {


	}

	MappingFunctionData::Input InterpolationMappingData::GetSupportedInput() const {

		return MappingFunctionData::Input::Numeric;
	}

	MappingFunctionData::Output InterpolationMappingData::GetSupportedOutput() const {

		return MappingFunctionData::Output::NumericAndColor;
	}

	double InterpolationMappingData::Interpolate(const DoubleMinDiff& outputMinDiff, double inputMin, double inputMax, double input) const {

		double inputDiff = inputMax - inputMin;
		return Interpolate(outputMinDiff.GetMin(), outputMinDiff.GetDiff(), inputMin, inputDiff, input);
	}

	GlyphColor InterpolationMappingData::Interpolate(const ColorMinDiff& outputMinDiff, double inputMin, double inputMax, double input) const {

		double inputDiff = inputMax - inputMin;

		GlyphColor minHSV = GlyphColor::ConvertRGBtoHSV(outputMinDiff.GetMin());
		GlyphColor maxHSV = GlyphColor::ConvertRGBtoHSV(outputMinDiff.GetMax());
		GlyphColor diffHSV(GlyphColor::HSV);
		diffHSV.Set(maxHSV[0] - minHSV[0], maxHSV[1] - minHSV[1], maxHSV[2] - minHSV[2]);

		GlyphColor outputHSV(GlyphColor::HSV);
		outputHSV.Set(0, Interpolate(minHSV[0], diffHSV[0], inputMin, inputDiff, input));
		outputHSV.Set(1, Interpolate(minHSV[1], diffHSV[1], inputMin, inputDiff, input));
		outputHSV.Set(2, Interpolate(minHSV[2], diffHSV[2], inputMin, inputDiff, input));

		return GlyphColor::ConvertHSVtoRGB(outputHSV);
	}

	double InterpolationMappingData::Interpolate(double outputMin, double outputDifference, double inputMin, double inputDifference, double input) const {

		if (std::abs(inputDifference) > 0.001) {

			if (m_function == MappingFunctionData::Function::LinearInterpolation) {

				return (outputMin + (((input - inputMin) / inputDifference) * outputDifference));
			}
			else if (m_function == MappingFunctionData::Function::LogarithmicInterpolation) {

				return (outputMin + ((std::log10(input - inputMin + 1) / std::log10(inputDifference + 1)) * outputDifference));
			}
		}
		
		return outputMin;
	}

	/*
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
	template class InterpolationMappingFunction < GlyphColor >;*/

} //namespace SynGlyphX