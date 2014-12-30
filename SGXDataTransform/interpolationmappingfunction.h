#ifndef SYNGLYPHX_INTERPOLATIONMAPPINGFUNCTION_H
#define SYNGLYPHX_INTERPOLATIONMAPPINGFUNCTION_H

#include "datamappingfunction.h"

namespace SynGlyphX {

	template<typename OutputType>
	class InterpolationMappingFunction : public DataMappingFunction<OutputType, double>
	{
	public:
		InterpolationMappingFunction();
		virtual ~InterpolationMappingFunction();

		void SetOutputMinAndDifference(OutputType min, OutputType difference);
		void SetInputMinAndDifference(double min, double difference);

	protected:
		virtual OutputType MapCombinedInput(double input);
		virtual double Interpolate(double input, double inputMin, double inputDifference, double outputMin, double outputDifference) const = 0;

		OutputType m_outputMin;
		OutputType m_outputDifference;
		double m_inputMin;
		double m_inputDifference
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_INTERPOLATIONMAPPINGFUNCTION_H