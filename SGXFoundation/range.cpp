#include "range.h"
#include <stdexcept>

namespace SynGlyphX {

	Range::Range(double min, double max) :
		m_min(min),
		m_max(max)
	{
		if (m_min >= m_max) {

			throw std::invalid_argument("Range object must have a min less than its max.");
		}
	}

	Range::Range(const Range& range) :
		m_min(range.m_min),
		m_max(range.m_max) {


	}

	Range::~Range()
	{
	}

	Range& Range::operator=(const Range& range) {

		m_min = range.m_min;
		m_max = range.m_max;

		return *this;
	}

	bool Range::operator==(const Range& range) const {

		return ((m_min == range.m_min) && (m_max == range.m_max));
	}

	bool Range::operator!=(const Range& range) const {

		return !operator==(range);
	}

	bool Range::operator<(const Range& range) const {

		return (m_max <= range.m_min);
	}

	bool Range::operator<(double value) const {

		return IsValueInRange(value);
	}

	double Range::GetMin() const {

		return m_min;
	}

	double Range::GetMax() const {

		return m_max;
	}

	bool Range::IsValueInRange(double value) const {

		return ((value >= m_min) && (value < m_max));
	}

} //namespace SynGlyphX