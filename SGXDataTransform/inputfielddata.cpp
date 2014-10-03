#include "inputfielddata.h"

namespace SynGlyphX {

	InputFieldData::InputFieldData(const QVariantList& data, double min, double max) :
		m_data(data),
		m_min(min),
		m_max(max),
		m_maxMinDifference(max - min)
	{

	}

	InputFieldData::~InputFieldData()
	{

	}

	const QVariantList& InputFieldData::GetData() const {

		return m_data;
	}

	double InputFieldData::GetMin() const {

		return m_min;
	}

	double InputFieldData::GetMax() const {

		return m_max;
	}

	double InputFieldData::GetMaxMinDifference() const {

		return m_maxMinDifference;
	}

} //namespace SynGlyphX