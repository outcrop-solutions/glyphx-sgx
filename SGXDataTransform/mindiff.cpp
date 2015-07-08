#include "mindiff.h"

namespace SynGlyphX {

	template<typename DataType>
	MinDiff<DataType>::MinDiff() :
		m_min(DataType()),
		m_diff(DataType())
	{
	}

	template<typename DataType>
	MinDiff<DataType>::MinDiff(DataType min, DataType diff) :
		m_min(min),
		m_diff(diff) {

	}

	template<typename DataType>
	MinDiff<DataType>::MinDiff(const MinDiff& minDiff) :
		m_min(minDiff.m_min),
		m_diff(minDiff.m_diff) {
	
	}

	template<typename DataType>
	MinDiff<DataType>::~MinDiff()
	{
	}

	template<typename DataType>
	MinDiff<DataType>& MinDiff<DataType>::operator=(const MinDiff& minDiff) {

		m_min = minDiff.m_min;
		m_diff = minDiff.m_diff;

		return *this;
	}

	template<typename DataType>
	bool MinDiff<DataType>::operator==(const MinDiff& minDiff) const {

		return ((m_min == minDiff.m_min) && (m_diff == minDiff.m_diff));
	}

	template<typename DataType>
	bool MinDiff<DataType>::operator!=(const MinDiff& minDiff) const {

		return !operator==(minDiff);
	}

	template<typename DataType>
	void MinDiff<DataType>::SetMinDiff(DataType min, DataType diff) {

		m_min = min;
		m_diff = diff;
	}

	template<typename DataType>
	void MinDiff<DataType>::SetMinMax(DataType min, DataType max) {

		m_min = min;
		m_diff = max - min;
	}

	template<typename DataType>
	void MinDiff<DataType>::SetMinWithCurrentMax(DataType min) {

		SetMinMax(min, m_min + m_diff);
	}

	template<typename DataType>
	void MinDiff<DataType>::SetMaxWithCurrentMin(DataType max) {

		m_diff = max - m_min;
	}

	template<typename DataType>
	DataType MinDiff<DataType>::GetMin() const {

		return m_min;
	}

	template<typename DataType>
	DataType MinDiff<DataType>::GetDiff() const {

		return m_diff;
	}

	template<typename DataType>
	DataType MinDiff<DataType>::GetMax() const {

		return m_min + m_diff;
	}

	template class MinDiff < int >;
	template class MinDiff < double >;
	template class MinDiff < GlyphColor >;

} //namespace SynGlyphX