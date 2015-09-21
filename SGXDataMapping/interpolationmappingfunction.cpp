#include "interpolationmappingfunction.h"
#include "glyphcolor.h"

namespace SynGlyphX {
	
	InterpolationMappingData::InterpolationMappingData(bool useLogarithmic) :
		MappingFunctionData(useLogarithmic ? MappingFunctionData::Function::LogarithmicInterpolation : MappingFunctionData::Function::LinearInterpolation),
		m_inputMinMaxType(InputMinMaxType::BoundInputField),
		m_userSpecifiedInputMinMax(),
		m_inputMinMaxFieldGroup(L"") {


	}

	InterpolationMappingData::InterpolationMappingData(const boost::property_tree::wptree& propertyTree) :
		MappingFunctionData(propertyTree) {


	}

	InterpolationMappingData::InterpolationMappingData(const InterpolationMappingData& data) :
		MappingFunctionData(data),
		m_inputMinMaxType(data.m_inputMinMaxType),
		m_userSpecifiedInputMinMax(data.m_userSpecifiedInputMinMax),
		m_inputMinMaxFieldGroup(data.m_inputMinMaxFieldGroup) {


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

	InterpolationMappingData::InputMinMaxType InterpolationMappingData::GetInputMinMaxType() const {

		return m_inputMinMaxType;
	}

	void InterpolationMappingData::SetInputMinMaxToBoundField() {

		m_inputMinMaxType = InputMinMaxType::BoundInputField;
	}

	void InterpolationMappingData::SetUserSpecifiedInputMinMax(const DoubleMinDiff& minMax) {

		m_inputMinMaxType = InputMinMaxType::UserSpecified;
		m_userSpecifiedInputMinMax = minMax;
	}

	const DoubleMinDiff& InterpolationMappingData::GetUserSpecifiedInputMinMax() const {

		if (m_inputMinMaxType != InputMinMaxType::UserSpecified) {

			throw std::exception("Can't get user specified input min/max value since user specified is not the input min/max type");
		}

		return m_userSpecifiedInputMinMax;
	}

	void InterpolationMappingData::SetInputMinMaxFieldGroup(const DataTransformMapping::FieldGroupName& minMaxFieldGroup) {

		m_inputMinMaxType = InputMinMaxType::InputFieldGroup;
		m_inputMinMaxFieldGroup = minMaxFieldGroup;
	}

	const DataTransformMapping::FieldGroupName& InterpolationMappingData::GetInputMinMaxFieldGroup() const {

		if (m_inputMinMaxType != InputMinMaxType::InputFieldGroup) {

			throw std::exception("Can't get field group input min/max value since input field group is not the input min/max type");
		}

		return m_inputMinMaxFieldGroup;
	}

} //namespace SynGlyphX